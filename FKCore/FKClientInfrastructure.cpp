#include "FKClientInfrastructure.h"

#include "FKConnectionManager.h"
#include "FKClientInfrastructureConnectionManager.h"
#include "FKBasicEvent.h"
#include "FKUpdateChannel.h"

#include "FKAusviceData.h"
#include "FKRoomData.h"
#include "FKRoomInviteData.h"
#include "FKRoomSettings.h"
#include "FKRoomContextFlags.h"
#include "FKInstructionSubjects.h"
#include "FKBasicEventSubjects.h"
#include "FKLogger.h"


/*!
\class FKClientInfrastructure
\brief This is the client-side infrastructure that manage client-side data
*/


FKClientInfrastructure::FKClientInfrastructure(QObject *parent):
        FKInfrastructure(parent){
    FK_CBEGIN
    connect(this,&FKClientInfrastructure::waitingForAnswerChanged,this,&FKClientInfrastructure::splitWaitingAnswer);
    FK_CEND
}

FKClientInfrastructure::~FKClientInfrastructure(){
    FK_DBEGIN
    FK_DEND
}

FKInfrastructureType FKClientInfrastructure::infrastructureType() const{
    return FKInfrastructureType::Client;
}

bool FKClientInfrastructure::isConnectedToRealm(){
    return _realmConnection && _realmConnection->isActive();
}

void FKClientInfrastructure::dropInfrastructure(){
    _realmConnection->dropConnection();
    _realmConnection->deleteLater();
    _realmConnection=nullptr;
    _logged=false;
    cancelAnswer(FKInfrastructureType::Realm);
}

void FKClientInfrastructure::requestLoginRealm(const QString id, const QString password){
    if(id.isEmpty() || password.isEmpty()){
        emit messageRequested(QString(tr("Unable login: empty id or password")));
        return;
    }

    if(!isConnectedToRealm()){
        emit messageRequested(QString(tr("Unable login: not connected to realm")));
        return;
    }

    if(_logged){
        emit messageRequested(QString(tr("Unable login: already logged")));
        return;
    }

    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::login)){
        emit messageRequested(QString(tr("Unable login: another request in progress")));
        return;
    }

    FKAusviceData data(id,password);
    FKBasicEvent ev(FKBasicEventSubject::login,data.toVariant());
    _realmConnection->sendGuestEvent(&ev);
}

void FKClientInfrastructure::submitLoginRealm(const QVariant& value){
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::login)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::submitLoginRealm()")
    }
    bool t=value.toBool();
    if(t){
        _logged=true;
        todo; //_customServer=id;
        emit loggedIn();
    }else{
        emit messageRequested(QString(tr("Realm declined login command")));
    }
}

void FKClientInfrastructure::submitLoginServer(const QVariant& value){
    if(!submitAnswer(FKInfrastructureType::Server,FKBasicEventSubject::login)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::submitLoginServer()")
    }
    FKRoomData data(value);
    QList<QVariant> users(data.value(FKRoomSettings::userList).toList());
    if(!users.isEmpty()){
        data.setValue(FKRoomSettings::contextFlags,FKRoomContextFlag::user);
        _serverLogged=true;
        for(auto u=users.constBegin();u!=users.constEnd();++u){
            startUser(data,(*u).toInt());
        }
        if(!requestAnswer(FKInfrastructureType::Server,FKBasicEventSubject::sync)){
            FK_MLOG("Unexpected behaivour in FKClientInfrastructure::submitLoginServer() sync wait")
        }
        emit loggedInServer();
    }else{
        emit messageRequested(QString(tr("Server declined login command")));
        todo; //handle this
    }
}

void FKClientInfrastructure::syncComplete(const QVariant& value){
    if(!submitAnswer(FKInfrastructureType::Server,FKBasicEventSubject::sync)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::syncComplete()")
    }
    qint32 reciever=value.toInt();
    emit messageRequested(QString(tr("Sync complete event recieved for %1 room context")).arg(QString::number(reciever)));
    FKInstructionObject syncInstruction(reciever,FKInstructionSubject::sync,QVariant());
    emit roomInstruction(syncInstruction);
}

bool FKClientInfrastructure::requestUserCreation(const QString& name){
    if(name.isEmpty()){
        emit messageRequested(QString(tr("Unable create user: name is empty")));
        return false;
    }
    if(!_logged){
        emit messageRequested(QString(tr("Unable create user: infrastructure is not logged on realm")));
        return false;
    }
    if(userPool().contains(name)){
        emit messageRequested(QString(tr("Unable create user: already exists")));
        return false;
    }
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::createUser)){
        emit messageRequested(QString(tr("Unable create user: another request in progress")));
        return false;
    }
    FKBasicEvent ev(FKBasicEventSubject::createUser,name);
    _realmConnection->sendBasicEvent(&ev);
    return true;
}

bool FKClientInfrastructure::requestUserDeletion(const QString& name){
    if(name.isEmpty()){
        emit messageRequested(QString(tr("Unable delete user: name is empty")));
        return false;
    }
    if(!_logged){
        emit messageRequested(QString(tr("Unable delete user: infrastructure is not logged on realm")));
        return false;
    }
    if(!userPool().contains(name)){
        emit messageRequested(QString(tr("Unable delete user: not exists")));
        return false;
    }
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::deleteUser)){
        emit messageRequested(QString(tr("Unable delete user: another request in progress")));
        return false;
    }
    FKBasicEvent ev(FKBasicEventSubject::deleteUser,name);
    _realmConnection->sendBasicEvent(&ev);
    return true;
}

void FKClientInfrastructure::requestRoomList(/*filters*/){
    if(!_logged){
        emit messageRequested(QString(tr("Unable request room list: infrastructure is not logged on realm")));
        return;
    }
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::roomList)){
        emit messageRequested(QString(tr("Unable request room list: another request in progress")));
        return;
    }
    QVariant filters;
    FKBasicEvent ev(FKBasicEventSubject::roomList,filters);
    _realmConnection->sendBasicEvent(&ev);
}

void FKClientInfrastructure::requestCreateRoom(const QString& roomName, const QString& roomType, const QStringList& userList,const bool custom){
    QString error;
    bool check=true;
    if(roomName.isEmpty()){
        error=QString(tr("Unable create room: name is empty"));
        check=false;
    }else if(roomType.isEmpty()){
        error=QString(tr("Unable create room: roomType is empty"));
        check=false;
    }else if(userList.isEmpty()){
        error=QString(tr("Unable create room: user list is empty"));
        check=false;
    }else if(!_logged){
        error=QString(tr("Unable create room: client infrastructure is not logged"));
        check=false;
    }else if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::createRoom)){
        error=QString(tr("Unable create room: another request in progress"));
        check=false;
    }

    if(check){
        FKRoomRequestData data(roomName,roomType,userList,custom);
        FKBasicEvent ev(FKBasicEventSubject::createRoom,data.toVariant());
        _realmConnection->sendBasicEvent(&ev);
    }else{
        emit messageRequested(error);
    }
}

void FKClientInfrastructure::requestJoinRoom(const QString& roomName, const QStringList& users){
    QString error;
    bool check=true;
    if(roomName.isEmpty()){
        error=QString(tr("Unable join room: name is empty"));
        check=false;
    }else if(users.isEmpty()){
        error=QString(tr("Unable join room: user list is empty"));
        check=false;
    }else if(!_logged){
        error=QString(tr("Unable join room: client infrastructure is not logged"));
        check=false;
    }else if(hasRoom()){
        error=QString(tr("Unable join room: another room exist on client"));
        check=false;
    }else if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::createRoom)){ //yes, it is correct, create room answer waiting
        error=QString(tr("Unable join room: another request in progress"));
        check=false;
    }

    if(check){
        FKRoomRequestData data(roomName,users);
        FKBasicEvent ev(FKBasicEventSubject::joinRoom,data.toVariant());
        _realmConnection->sendBasicEvent(&ev);
    }else{
        emit messageRequested(error);
    }
}

void FKClientInfrastructure::refreshUserList(const QVariant& value){
    QStringList lst(value.toStringList());
    _userPool=lst;
    emit userPoolChanged();
}

void FKClientInfrastructure::respondUserCreation(const QVariant& value){
    const QString name=value.toString();
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::createUser)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::respondUserCreation()")
    }
    if(!name.isEmpty()){
        _userPool.append(name);
        emit userPoolChanged();
    }
}

void FKClientInfrastructure::respondUserDeletion(const QVariant& value){
    const QString name=value.toString();
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::deleteUser)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::respondUserDeletion()")
    }
    if(!name.isEmpty()){
        _userPool.removeOne(name);
        emit userPoolChanged();
    }
}

void FKClientInfrastructure::respondRoomList(const QVariant& value){
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::roomList)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::respondRoomList()")
    }
    _roomList=value.toStringList();
    emit roomListChanged();
}

void FKClientInfrastructure::respondCreateRoom(const QVariant& data){
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::createRoom)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::respondCreateRoom()")
    }
    QString msg(tr("Create room request recieved"));
    emit messageRequested(msg);
    bool answer=false;
    if(hasRoom()){
        msg=QString(tr("Create room request recieved, but room does exists"));
        answer=false;
    }else{
        FKRoomInviteData roomData(data);
        answer=roomData.isValid();
        if(!answer){
            msg=QString(tr("Invalid create room request recieved"));
        }else{
            _roomData=roomData;
            FKInstructionObject loadInstruction(FKInstructionSubject::loadModule,roomData.roomType());
            emit roomInstruction(loadInstruction);
        }
    }
    if(!answer){
        emit messageRequested(QString(tr("Room creation failed")));
        roomStopped();
    }
}

//void FKClientInfrastructure::respondCustomServer(const QVariant& value){
//    todo; //refactoring
//    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::customServer)){
//        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::respondCustomServer()")
//    }
//    const FKAusviceData data(value);
//    QString error;
//    qint32 serverId;
//    QString password;
//    if(!data.isValid()){
//        error=QString(tr("Unable create custom server: request rejected by realm"));
//    }else{
//        serverId=data.serverId();
//        password=data.password();
//        if(serverId<=0){
//            error=QString(tr("Unable create custom server: realm provided no server id"));
//        }else if(password.isEmpty()){
//            error=QString(tr("Unable create custom server: realm provided no server password"));
//        }
//    }

//    if(error.isEmpty()){
//        emit customServerRequested(serverId,password);
//    }else{
//        emit messageRequested(error);
//    }
//}

void FKClientInfrastructure::incomeVersionData(const QVariant& value){
    todo;
//    if(!_roomModule){
//        FK_MLOG("invalid income version list data case: no room module loaded")
//        return;
//    }
//    const FKVersionList newVersion(value.value<FKVersionList>());
//    if(!newVersion.isValid()){
//        FK_MLOG("invalid income version list data")
//        return;
//    }
//    _updates.channels.clear();
//    const FKVersionList currentVersion(_roomModule->version());

//    const QList<FKVersionList::Data> newList(newVersion.enteties());
//    const QList<FKVersionList::Data> currentList(currentVersion.enteties());
//    QList<QPair<QString,qint8>> filesList;
//    for(auto v=newList.constBegin();v!=newList.constEnd();++v){
//        filesList.append(QPair<QString,qint8>(v->path,v->platform));
//    }
//    for(auto v=currentList.constBegin();v!=currentList.constEnd();++v){
//        const QPair<QString,qint8> pair(v->path,v->platform);
//        if(!filesList.contains(pair))filesList.append(pair);
//    }

//    for(auto f=filesList.constBegin();f!=filesList.constEnd();++f){
//        auto newInfo=newVersion.getInfo(f->first,f->second);
//        auto currentInfo=currentVersion.getInfo(f->first,f->second);
//        _updates.channels.append(QSharedPointer<FKUpdateChannel>(new FKUpdateChannel(f->first,currentInfo.first,f->second,newInfo.first,newInfo.second)));
//    }
    //emit updateListChanged();
}

QStringList FKClientInfrastructure::userPool() const{
    return _userPool;
}

//QStringList FKClientInfrastructure::activeUsers() const{
//    return _users.keys();
//}

QList<QSharedPointer<FKUpdateChannel> > FKClientInfrastructure::updates() const{
    return _updates.channels;
}

void FKClientInfrastructure::messageFromRealm(const QString& msg){
    emit messageRequested(QString(tr("Realm->%1")).arg(msg));
}

void FKClientInfrastructure::setCustomServerId(const qint32 serverId){
    _customServerId=serverId;
}

bool FKClientInfrastructure::waitingForRealmAnswer() const{
    return FKInfrastructure::waitingForAnswer(FKInfrastructureType::Realm);
}

bool FKClientInfrastructure::waitingForServerAnswer() const{
    return FKInfrastructure::waitingForAnswer(FKInfrastructureType::Server);
}

void FKClientInfrastructure::setRealmConnectionSettings(const QString ip, const qint32 port){
    _realmIP=ip;
    _realmPort=port;
}

QString FKClientInfrastructure::realmIP() const{
    return _realmIP;
}

qint32 FKClientInfrastructure::realmPort() const{
    return _realmPort;
}

void FKClientInfrastructure::realmConnection(FKConnector* connector){
    _realmConnection=new FKClientInfrastructureConnectionManager(this,connector,this);
    connect(_realmConnection,SIGNAL(connectionStatusChanged()),SLOT(connectorStatusChanged()));
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::connect)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::realmConnection()")
    }
}

void FKClientInfrastructure::serverConnection(FKConnector* connector){
    _serverConnection=new FKClientInfrastructureConnectionManagerS(this,connector,this);
    connect(_serverConnection,SIGNAL(connectionStatusChanged()),SLOT(serverConnectorStatusChanged()));
    if(!requestAnswer(FKInfrastructureType::Server,FKBasicEventSubject::connect)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::serverConnection()")
    }
}

void FKClientInfrastructure::handleRoomInstruction(FKInstructionObject instruction){
    const qint32 instructionType=instruction.subject();
    if(instructionType==FKInstructionSubject::loadModule){
        FKVersionList loadedVersion(instruction.value().value<FKVersionList>());
        if(loadedVersion.isValid()){
            if(_roomData.isValid()){
                if(!_roomVersion.isValid()){
                    _roomVersion=loadedVersion;
                    emit connectToServerRequest(_roomData.address(),_roomData.port());
                }else{
                    todo;
                }
            }else{
                todo;
            }
        }else{
            todo;
        }
    }else{
        todo;
    }
}

void FKClientInfrastructure::connectorStatusChanged(){
    if(_realmConnection->isActive()){
        emit connectedToRealm();
        if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::connect)){
            FK_MLOG("Unexpected behaivour in FKClientInfrastructure::connectorStatusChanged()")
        }
    }else{
        _logged=false;
        emit disconnectedFromRealm();
        cancelAnswer(FKInfrastructureType::Realm);
    }
}

void FKClientInfrastructure::serverConnectorStatusChanged(){
    if(_serverConnection->isActive()){
        emit connectedToServer();
        if(!submitAnswer(FKInfrastructureType::Server,FKBasicEventSubject::connect)){
            FK_MLOG("Unexpected behaivour in FKClientInfrastructure::serverConnectorStatusChanged()")
        }
        requestLoginServer();
    }else{
        _serverLogged=false;
        emit disconnectedFromServer();
        cancelAnswer(FKInfrastructureType::Server);
    }
}

void FKClientInfrastructure::splitWaitingAnswer(FKInfrastructureType t){
    if(t==FKInfrastructureType::Realm){
        emit waitingForRealmAnswerChanged();
    }else if(t==FKInfrastructureType::Server){
        emit waitingForServerAnswerChanged();
    }
}

bool FKClientInfrastructure::hasRoom()const{
    return _roomData.isValid();
}

void FKClientInfrastructure::roomStopped(){
    todo;
}

void FKClientInfrastructure::requestLoginServer(){
    if(!requestAnswer(FKInfrastructureType::Server,FKBasicEventSubject::login)){
        emit messageRequested(QString(tr("Unable login: another request in progress")));
        return;
    }

    FKAusviceData data(_roomData.client(),_roomData.password());
    FKBasicEvent ev(FKBasicEventSubject::login,data.toVariant());
    _serverConnection->sendGuestEvent(&ev);
}

void FKClientInfrastructure::startUser(const FKRoomData& data,const qint32 objectId){
    if(objectId<=0){
        emit messageRequested(QString(tr("Unable start context: object id not recognized")));
        return;
    }
    emit messageRequested(QString(tr("Room context %1 requested")).arg(QString::number(objectId)));
    FKInstructionObject createContextInstruction(objectId,FKInstructionSubject::createContext,data.toVariant());
    emit roomInstruction(createContextInstruction);
}

/*!
 * \fn void FKClientInfrastructure::connectedToRealm()
 * \brief Signal emitted when infrastructure successfully connects as guest to realm
 */

/*!
 * \fn void FKClientInfrastructure::disconnectedFromRealm()
 * \brief Signal emitted when infrastructure disconnects from realm
 */

/*!
 * \fn void FKClientInfrastructure::loggedIn()
 * \brief Signal emitted when infrastructure recieve ausvice submit from realm
 */


