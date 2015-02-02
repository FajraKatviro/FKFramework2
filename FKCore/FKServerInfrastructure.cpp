#include "FKServerInfrastructure.h"

#include "FKUserInfrastructureSlot.h"
#include "FKServerConnectionManager.h"
#include "FKBasicEvent.h"
#include "FKObjectManager.h"
#include "FKAusviceData.h"
#include "FKRoomData.h"

#include "FKBasicEventSubjects.h"
#include "FKLogger.h"

FKServerInfrastructure::FKServerInfrastructure(QObject *parent):
        FKInfrastructure(parent),_logged(false),_realmConnection(0),_room(0),_idgenerator(1){
    FK_CBEGIN
    //_om=new FKObjectManager(this);
    FK_CEND
}

FKServerInfrastructure::~FKServerInfrastructure(){
    FK_DBEGIN
    FK_DEND
}

FKInfrastructureType FKServerInfrastructure::infrastructureType() const{
    return FKInfrastructureType::Server;
}

bool FKServerInfrastructure::isConnectedToRealm(){
    return _logged;
}

void FKServerInfrastructure::dropInfrastructure(){
    for(auto i=_invitedUsers.begin();i!=_invitedUsers.end();++i){
//        i.value()->dropUser();
//        i.value()->deleteLater();
    }
    _invitedUsers.clear();
    for(auto i=_clients.begin();i!=_clients.end();++i){
//        i.value()->dropClient();
//        i.value()->deleteLater();
    }
    _clients.clear();
    _realmConnection->dropConnection();
    _realmConnection->deleteLater();
    _realmConnection=0;
    _logged=false;
    cancelAnswer(FKInfrastructureType::Realm);
}

void FKServerInfrastructure::requestLoginRealm(const qint32 id, const QString& password){
    if(id<=0 || password.isEmpty()){
        emit messageRequested(QString(tr("Unable login: invalid id or password")));
        return;
    }

    if(!checkRealm())return;

    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::login)){
        emit messageRequested(QString(tr("Unable login: another request in progress")));
        return;
    }

    FKBasicEvent ev(FKBasicEventSubject::login,FKAusviceData(id,password).toVariant());
    _realmConnection->sendGuestEvent(&ev);
}

void FKServerInfrastructure::submitLoginRealm(const QVariant& value){
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::login)){
        FK_MLOG("Unexpected behaivour in FKServerInfrastructure::submitLoginRealm()")
    }
    bool t=value.toBool();
    if(t){
        _logged=true;
        emit loggedIn();
    }else{
        emit messageRequested(QString(tr("Realm declined login command")));
    }
}

void FKServerInfrastructure::registerRoomTypeRequest(const QString& roomType){
    if(roomType.isEmpty()){
        emit messageRequested(QString(tr("Unable register empty room type")));
        return;
    }
    if(!checkRealm())return;
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::registerRoomType)){
        emit messageRequested(QString(tr("Unable register room type: another request in progress")));
        return;
    }
    FKBasicEvent request(FKBasicEventSubject::registerRoomType,roomType);
    _realmConnection->sendBasicEvent(&request);
}

void FKServerInfrastructure::registerRoomTypeRespond(const QVariant& value)
{
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::registerRoomType)){
        FK_MLOG("Unexpected behavior in FKServerInfrastructure::registerRoomTypeRespond()")
    }
    if(value.toBool()){
        emit messageRequested(QString(tr("Room type registration success")));
    }else{
        emit messageRequested(QString(tr("Room type not registered")));
    }
}

void FKServerInfrastructure::createRoomRequested(const QVariant& data){
    bool answer;
    if(hasRoom()){
        FK_MLOG("Create room request recieved, but room does exists")
        answer=false;
    }else{
        const FKRoomData roomData(data);
        answer=roomData.isValid();
        if(!answer){
            FK_MLOG("Invalid create room request recieved")
        }else{
            answer=createRoom(roomData);
        }
    }
    FKBasicEvent ev(FKBasicEventSubject::createRoom,answer);
    _realmConnection->sendBasicEvent(&ev);
}

//void FKServerInfrastructure::clientInvited(const QVariant& data){
//    QString client;
//    QMap<QString,QString> userMap;
//    bool answer=checkInviteData(data,client,userMap);
//    if(answer){
//        FKClientInfrastructureSlot* clientSlot=new FKClientInfrastructureSlot(this);
//        _invitedClients.insert(client,clientSlot);
//        for(auto u=userMap.constBegin();u!=userMap.constEnd();++u){
//            const qint32 id=_idgenerator.take();
//            FKUserInfrastructureSlot* userSlot=new FKUserInfrastructureSlot(clientSlot,id,u.value(),clientSlot);
//            clientSlot->addUser(userSlot);
//            _invitedUsers.insert(u.key(),userSlot);
//        }
//    }
//    FKBasicEvent ev(FKBasicEventSubject::userList,answer);
//    _realmConnection->sendBasicEvent(&ev);
//}

void FKServerInfrastructure::messageFromRealm(const QString& msg){
    emit messageRequested(QString(tr("Realm -> server: %1")).arg(msg));
}

void FKServerInfrastructure::realmConnection(FKConnector* connector){
    _realmConnection=new FKServerConnectionManagerR(this,connector,this);
    connect(_realmConnection,SIGNAL(connectionStatusChanged()),SLOT(connectorStatusChanged()));
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::connect)){
        FK_MLOG("Unexpected behaivour in FKServerInfrastructure::realmConnection()")
    }
}

//bool FKServerInfrastructure::checkInviteData(const QVariant& data, QString& client, QMap<QString, QString>& userMap){
//    QMap<QString,QVariant> clientData=data.toMap();
//    client=clientData.value(FKAusviceIdentifiers::client).toString();
//    if(client.isEmpty()){
//        FK_MLOG("Unexpected empty client name in FKServerInfrastructure::clientInvited()")
//        return false;
//    }
//    if(_clients.contains(client)){
//        FK_MLOG("Unexpected duplicate client invite in FKServerInfrastructure::clientInvited()")
//        return false;
//    }
//    const QList<QVariant> users=clientData.value(FKAusviceIdentifiers::users).toList();
//    if(users.isEmpty()){
//        FK_MLOG("Unexpected empty user list in FKServerInfrastructure::clientInvited()")
//        return false;
//    }
//    const QList<QVariant> passwords=clientData.value(FKAusviceIdentifiers::password);
//    if(users.size()!=passwords.size()){
//        FK_MLOG("Unexpected invalid password list in FKServerInfrastructure::clientInvited()")
//        return false;
//    }
//    for(auto u=users.constBegin(), p=passwords.constBegin();u!=users.constEnd();++u,++p){
//        QString userName=u->toString();
//        if(userName.isEmpty()){
//            FK_MLOG("Unexpected empty user name in FKServerInfrastructure::clientInvited()")
//            return false;
//        }
//        if(userMap.contains(userName)){
//            FK_MLOG("Unexpected duplicate user name in FKServerInfrastructure::clientInvited()")
//            return false;
//        }
//        if(_users.contains(userName)){
//            FK_MLOG("Unexpected duplicate user invite in FKServerInfrastructure::clientInvited()")
//            return false;
//        }
//        QString password=p->toString();
//        if(password.isEmpty()){
//            FK_MLOG("Unexpected empty password in FKServerInfrastructure::clientInvited()")
//            return false;
//        }
//        userMap.insert(userName,password);
//    }
//    if(!newClientAllowed(userMap.size())){
//        FK_MLOGV("New users is not allowed for current room",userMap.size())
//        return false;
//    }
//    return true;
//}

void FKServerInfrastructure::realmConnectorStatusChanged(){
    if(_realmConnection->isActive()){
        emit connectedToRealm();
        if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::connect)){
            FK_MLOG("Unexpected behaivour in FKServerInfrastructure::connectorStatusChanged()")
        }
    }else{
        _logged=false;
        emit disconnectedFromRealm();
        cancelAnswer(FKInfrastructureType::Realm);
    }
}

void FKServerInfrastructure::roomDataChanged(const qint32 maxActors, const qint32 actors, const qint32 maxUsers, const qint32 users){
    if(!_logged){
        FK_MLOG("Warning! Unable send room data delta to realm: server is not logged in")
        return;
    }

    QVariant delta=FKRoomData::createDelta(maxActors    -_roomData.maximumActors(),
                                           actors       -_roomData.actors(),
                                           maxUsers     -_roomData.maximumUsers(),
                                           users        -_roomData.users());
    if(delta.isValid()){
        FKBasicEvent ev(FKBasicEventSubject::roomData,delta);
        _realmConnection->sendBasicEvent(&ev);
    }
}

void FKServerInfrastructure::roomStarted(){
    if(!_logged){
        FK_MLOG("Warning! Unable send room started event to realm: server is not logged in")
        return;
    }
    FKBasicEvent ev(FKBasicEventSubject::startRoom);
    _realmConnection->sendBasicEvent(&ev);
}

void FKServerInfrastructure::roomStopped(){
    if(!_logged){
        FK_MLOG("Warning! Unable send room stopped event to realm: server is not logged in")
        return;
    }
    FKBasicEvent ev(FKBasicEventSubject::stopRoom);
    _realmConnection->sendBasicEvent(&ev);
}

bool FKServerInfrastructure::checkRealm() const{
    if(!isConnectedToRealm()){
        emit messageRequested(QString(tr("Unable login: not connected to realm")));
        return false;
    }
    if(_logged){
        emit messageRequested(QString(tr("Unable login: already logged")));
        return false;
    }
    return true;
}

/*!
 * \fn void FKServerInfrastructure::connectedToRealm()
 * \brief Signal emitted when infrastructure successfully connects as guest to realm
 */

/*!
 * \fn void FKServerInfrastructure::disconnectedFromRealm()
 * \brief Signal emitted when infrastructure disconnects from realm
 */

/*!
 * \fn void FKServerInfrastructure::loggedIn()
 * \brief Signal emitted when infrastructure recieve ausvice submit from realm
 */


