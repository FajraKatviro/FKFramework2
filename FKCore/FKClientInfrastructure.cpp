#include "FKClientInfrastructure.h"

#include "FKConnectionManager.h"
#include "FKClientInfrastructureConnectionManager.h"
#include "FKBasicEvent.h"
#include "FKRoomModule.h"

#include "FKAusviceData.h"
#include "FKRoomInviteData.h"
#include "FKRoomData.h"
#include "FKBasicEventSubjects.h"
#include "FKLogger.h"

FKClientInfrastructure::FKClientInfrastructure(QObject *parent):
        FKInfrastructure(parent),_logged(false),_realmConnection(0),_serverConnection(0),_customServerId(-1){
    FK_CBEGIN
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
    for(auto i=_users.begin();i!=_users.end();++i){
        i.value()->dropUser();
        i.value()->deleteLater();
    }
    _users.clear();
    _realmConnection->dropConnection();
    _realmConnection->deleteLater();
    _realmConnection=nullptr;
    _logged=false;
    cancelAnswer(FKInfrastructureType::Realm);
}

void FKClientInfrastructure::requestLoginRealm(const QString& id, const QString& password){
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

    _clientId=id;

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
        emit loggedIn();
    }else{
        emit messageRequested(QString(tr("Realm declined login command")));
    }
}

void FKClientInfrastructure::submitLoginServer(const QVariant& value){
    if(!submitAnswer(FKInfrastructureType::Server,FKBasicEventSubject::login)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::submitLoginServer()")
    }
    QList<QVariant> users=value.toList();
    if(!users.isEmpty()){
        _serverLogged=true;
        for(auto u=users.constBegin();u!=users.constEnd();++u){
            startUser((*u).toInt());
        }
        emit loggedInServer();
    }else{
        emit messageRequested(QString(tr("Server declined login command")));
    }
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
    }else if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::joinRoom)){
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
    }else if(_roomModule){
        error=QString(tr("Unable join room: another room exist on client"));
        check=false;
    }else if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::joinRoom)){
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

void FKClientInfrastructure::respondCreateRoom(const QVariant& value){
    Q_UNUSED(value)
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::joinRoom)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::respondCreateRoom()")
    }

    emit messageRequested(QString(tr("Fail create room")));
}

void FKClientInfrastructure::respondEnterRoom(const QVariant& value){
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::joinRoom)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::respondEnterRoom()")
    }
    FKRoomInviteData invite(value);
    bool success=false;
    if(invite.isValid()){
        if(!_roomModule){
            _roomModule=new FKRoomModule(invite.roomType(),this);
            if(_roomModule->load()){
                success=true;
            }else{
                emit messageRequested(QString(tr("Unable load room module %1. Use chat room instead")).arg(invite.roomType()));
                _roomModule->deleteLater();
                _roomModule=nullptr;
            }
        }else{
            emit messageRequested(QString(tr("Unable load room module: another one is loaded")));
        }
    }else{
        emit messageRequested(QString(tr("Fail enter room: invalid invite data")));
    }

    if(success){
        _dynamicPassword=invite.password();
        emit connectToServerRequest(invite.address(),invite.port());
    }
}

void FKClientInfrastructure::respondCustomServer(const QVariant& value){
    todo; //refactoring
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::customServer)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::respondCustomServer()")
    }
    const FKAusviceData data(value);
    QString error;
    qint32 serverId;
    QString password;
    if(!data.isValid()){
        error=QString(tr("Unable create custom server: request rejected by realm"));
    }else{
        serverId=data.serverId();
        password=data.password();
        if(serverId<=0){
            error=QString(tr("Unable create custom server: realm provided no server id"));
        }else if(password.isEmpty()){
            error=QString(tr("Unable create custom server: realm provided no server password"));
        }
    }

    if(error.isEmpty()){
        emit customServerRequested(serverId,password);
    }else{
        emit messageRequested(error);
    }
}

void FKClientInfrastructure::incomeVersionData(const QVariant& value){
    todo;
}

QStringList FKClientInfrastructure::userPool() const{
    return _userPool;
}

QStringList FKClientInfrastructure::activeUsers() const{
    return _users.keys();
}

QList<QSharedPointer<FKUpdateChannel> > FKClientInfrastructure::updates() const{
    return _updates.channels;
}

void FKClientInfrastructure::messageFromRealm(const QString& msg){
    emit messageRequested(QString(tr("Realm -> client: %1")).arg(msg));
}

void FKClientInfrastructure::setCustomServerId(const qint32 serverId){
    _customServerId=serverId;
}

qint32 FKClientInfrastructure::realmPort() const{
    return 0;
}

QString FKClientInfrastructure::realmIP() const{
    return QString();
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

void FKClientInfrastructure::connectorStatusChanged(){
    if(_realmConnection->isActive()){
        emit connectedToRealm();
        if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::connect)){
            FK_MLOG("Unexpected behaivour in FKClientInfrastructure::connectorStatusChanged()")
        }
        requestLoginServer();
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
    }else{
        _serverLogged=false;
        emit disconnectedFromServer();
        cancelAnswer(FKInfrastructureType::Server);
    }
}

void FKClientInfrastructure::requestLoginServer(){
    if(!requestAnswer(FKInfrastructureType::Server,FKBasicEventSubject::login)){
        emit messageRequested(QString(tr("Unable login: another request in progress")));
        return;
    }

    FKAusviceData data(_clientId,_dynamicPassword);
    FKBasicEvent ev(FKBasicEventSubject::login,data.toVariant());
    _serverConnection->sendGuestEvent(&ev);
}

void FKClientInfrastructure::startUser(const qint32 objectId){
    if(objectId==0){
        emit messageRequested(QString(tr("Unable start user infrastructure: object id not recognized")));
        return;
    }
    if(_users.contains(objectId)){
        emit messageRequested(QString(tr("Unable start user infrastructure %1: already started")).arg(QString::number(objectId)));
        return;
    }
    FKUserInfrastructure* infr=new FKUserInfrastructure(this);
    _users.insert(objectId,infr);

    connect(infr,SIGNAL(messageRequested(QString)),SIGNAL(messageRequested(QString)));
    //FKDataBase* db=new FKFSDB(_server);
    //db->setPath(serverDatabasePath());
    //_server->setDataBase(db);
    emit messageRequested(QString(tr("User infrastructure started")));
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


