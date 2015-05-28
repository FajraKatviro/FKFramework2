#include "FKServerInfrastructure.h"

#include "FKUserInfrastructureSlot.h"
#include "FKServerConnectionManager.h"
#include "FKBasicEvent.h"
#include "FKObjectManager.h"
#include "FKAusviceData.h"
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

bool FKServerInfrastructure::isConnectedToRealm()const{
    return _realmConnection && _realmConnection->isActive();
}

void FKServerInfrastructure::dropInfrastructure(){
    for(auto i=_guests.begin();i!=_guests.end();++i){
//        i.value()->dropUser();
//        i.value()->deleteLater();
        //todo
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

void FKServerInfrastructure::clientInvited(const QVariant& data){
    FKRoomInviteData invite(data);
    QVariant ret;
    if(checkInviteData(invite)){
        FKRoomInviteData answer(invite.client(),userPort());
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
        _roomData.changeUsers(invite.users().count());
        roomDataChanged(_roomData.maximumActors(),_roomData.actors(),_roomData.maximumUsers(),_roomData.users());
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
                _room->setup(roomData);
                answer=true;
            }
        }else{
            FK_MLOGV("Unable load room module",roomData.roomType())
            _roomModule->deleteLater();
            _roomModule=0;
        }
    }
    return answer;
}

QString FKServerInfrastructure::createUserInvitePassword(){
    return createRandomString(8,10);
}

bool FKServerInfrastructure::checkInviteData(const FKRoomInviteData& data){
    if(data.isValid()){
        if(_roomData.maximumUsers()-_roomData.users()>=data.users().count()){
            return true;
        }
    }
    return false;
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


