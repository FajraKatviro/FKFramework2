#include "FKServerInfrastructure.h"

#include "FKUserInfrastructureSlot.h"
#include "FKServerConnectionManager.h"
#include "FKBasicEvent.h"
#include "FKObjectManager.h"
#include "FKAusviceData.h"
#include "FKRoomData.h"
#include "FKRoomInviteData.h"
#include "FKRoomData.h"
#include "FKFSDB.h"
#include "FKRoomModule.h"
#include "FKRoom.h"

#include "FKBasicEventSubjects.h"
#include "FKLogger.h"

FKServerInfrastructure::FKServerInfrastructure(QObject *parent):
        FKInfrastructure(parent),_logged(false),_realmConnection(0),_roomModule(0),_room(0),_idgenerator(1),_id(-1){
    FK_CBEGIN
    _om=new FKObjectManager(this);
    FK_CEND
}

FKServerInfrastructure::~FKServerInfrastructure(){
    FK_DBEGIN
    FK_DEND
}

FKInfrastructureType FKServerInfrastructure::infrastructureType() const{
    return FKInfrastructureType::Server;
}

bool FKServerInfrastructure::isConnectedToRealm()const{
    return _realmConnection && _realmConnection->isActive();
}

void FKServerInfrastructure::dropInfrastructure(){
    for(auto i=_guests.begin();i!=_guests.end();++i){
//        i.value()->dropUser();
//        i.value()->deleteLater();
        todo;
    }
    _guests.clear();
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
    _id=id;
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
    if(!_logged){
        emit messageRequested(QString(tr("Unable register room type: server is not not logged in")));
        return;
    }
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::registerRoomType)){
        emit messageRequested(QString(tr("Unable register room type: another request in progress")));
        return;
    }
    FKBasicEvent request(FKBasicEventSubject::registerRoomType,roomType);
    _realmConnection->sendBasicEvent(&request);
}

void FKServerInfrastructure::registerRoomTypeRespond(const QVariant& value){
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::registerRoomType)){
        FK_MLOG("Unexpected behavior in FKServerInfrastructure::registerRoomTypeRespond()")
    }
    if(value.toBool()){
        emit messageRequested(QString(tr("Room type registration success")));
    }else{
        emit messageRequested(QString(tr("Room type not registered")));
    }
}

void FKServerInfrastructure::removeRoomTypeRequest(const QString& roomType){
    if(roomType.isEmpty()){
        emit messageRequested(QString(tr("Unable remove empty room type")));
        return;
    }
    if(!_logged){
        emit messageRequested(QString(tr("Unable remove room type: server is not not logged in")));
        return;
    }
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::removeRoomType)){
        emit messageRequested(QString(tr("Unable remove room type: another request in progress")));
        return;
    }
    FKBasicEvent request(FKBasicEventSubject::removeRoomType,roomType);
    _realmConnection->sendBasicEvent(&request);
}

void FKServerInfrastructure::removeRoomTypeRespond(const QVariant& value){
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::removeRoomType)){
        FK_MLOG("Unexpected behavior in FKServerInfrastructure::removeRoomTypeRespond()")
    }
    if(value.toBool()){
        emit messageRequested(QString(tr("Room type removed")));
    }else{
        emit messageRequested(QString(tr("Room type not removed")));
    }
}

void FKServerInfrastructure::createRoomRequested(const QVariant& data){
    QString msg;
    bool answer;
    if(hasRoom()){
        msg=QString(tr("Create room request recieved, but room does exists"));
        answer=false;
    }else{
        const FKRoomData roomData(data);
        answer=roomData.isValid();
        if(!answer){
            msg=QString(tr("Invalid create room request recieved"));
        }else{
            answer=createRoom(roomData);
            if(answer){
                mgr=QString(tr("Failed create room of type %1")).arg(roomData.roomType());
            }else{
                mgr=QString(tr("Created room of type %1")).arg(roomData.roomType());
            }
        }
    }
    emit messageRequested(msg);
    if(!answer){
        FKBasicEvent ev(FKBasicEventSubject::stopRoom);
        _realmConnection->sendBasicEvent(&ev);
    }
}

void FKServerInfrastructure::clientInvited(const QVariant& data){
    FKRoomInviteData invite(data);
    QVariant ret;
    if(_room->addUsers(invite)){
        //FKRoomInviteData answer(invite.client(),userPort());
        todo; //delayed creation and id send, check reconnection on userConnection, create user object
        FKUserInfrastructureAlias* userAlias=new FKUserInfrastructureAlias(invite.client());
        _clients.insert(invite.client(),userAlias);
        foreach(QString u,invite.users()){
            const qint32 id=_idgenerator.take();
            FKUserInfrastructureSlot* userSlot=new FKUserInfrastructureSlot(userAlias,id,createUserInvitePassword());
            userAlias->addUser(userSlot);
            _users.insert(u,userSlot);
            answer.addUser(u,userSlot->password());
        }
        ret=answer.toVariant();
    }else{
        FKRoomInviteData answer(invite.client());
        ret=answer.toVariant();
    }
    FKBasicEvent ev(FKBasicEventSubject::joinRoom,ret);
    _realmConnection->sendBasicEvent(&ev);
}

void FKServerInfrastructure::messageFromRealm(const QString& msg){
    emit messageRequested(QString(tr("Realm -> server: %1")).arg(msg));
}

qint32 FKServerInfrastructure::serverId() const{
    return _id;
}

void FKServerInfrastructure::realmConnection(FKConnector* connector){
    _realmConnection=new FKServerConnectionManagerR(this,connector,this);
    connect(_realmConnection,SIGNAL(connectionStatusChanged()),SLOT(realmConnectorStatusChanged()));
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::connect)){
        FK_MLOG("Unexpected behaivour in FKServerInfrastructure::realmConnection()")
    }
}

void FKServerInfrastructure::realmConnectorStatusChanged(){
    if(_realmConnection->isActive()){
        if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::connect)){
            FK_MLOG("Unexpected behaivour in FKServerInfrastructure::connectorStatusChanged()")
        }
        emit connectedToRealm();
    }else{
        _logged=false;
        cancelAnswer(FKInfrastructureType::Realm);
        emit disconnectedFromRealm();
    }
}

void FKServerInfrastructure::roomDataChanged(const qint32 maxActorsDelta,const qint32 actorsDelta, const qint32 maxUsersDelta, const qint32 usersDelta){
    if(!_logged){
        FK_MLOG("Warning! Unable send room data delta to realm: server is not logged in")
        return;
    }

    QVariant delta=FKRoomData::createDelta(maxActorsDelta,actorsDelta,maxUsersDelta,usersDelta);
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

bool FKServerInfrastructure::checkRealm(){
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

bool FKServerInfrastructure::hasRoom() const{
    return _room;
}

bool FKServerInfrastructure::createRoom(const FKRoomData& roomData){
    bool answer=false;
    if(!_roomModule){
        _roomModule=new FKRoomModule(roomData.roomType(),this);
        if(_roomModule->load()){
            FKObject* room=_om->genObject(_roomModule->roomClass());
            _room=qobject_cast<FKRoom*>(room);
            if(!_room){
                FK_MLOGV("Invalid room module",roomData.roomType())
                if(room)_om->deleteObject(room);
                _roomModule->deleteLater();
                _roomModule=0;
            }else{
                _om->setRoomModule(_roomModule);
                connect(_room,SIGNAL(roomDataChanged(qint32,qint32,qint32,qint32)),SLOT(roomDataChanged(qint32,qint32,qint32,qint32)));
                _room->setup(roomData);
                FK_MLOG("room created on server")
                answer=true;
            }
        }else{
            FK_MLOGV("Unable load room module",roomData.roomType())
            _roomModule->deleteLater();
            _roomModule=0;
        }
    }else{
        FK_MLOG("room module is not empty")
    }
    return answer;
}



FKDataBase* FKServerInfrastructure::createRoomDatabase(){
    FKDataBase* db=new FKFSDB(this);
    db->setPath("../data/serverRoomDatabase");
    return db;
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


