#include "FKServerInfrastructure.h"

#include "FKClientInfrastructureReferent.h"
#include "FKServerConnectionManager.h"
#include "FKBasicEvent.h"
#include "FKAusviceData.h"
#include "FKRoomData.h"
#include "FKRoomInviteData.h"
#include "FKRoomData.h"
#include "FKFSDB.h"
#include "FKRoomModule.h"
#include "FKRoom.h"
#include "FKConnector.h"

#include "FKBasicEventSubjects.h"
#include "FKLogger.h"

/*!
\class FKServerInfrastructure
\brief Infrastructure provides server-side data handle, create room and add clients to room
*/

FKServerInfrastructure::FKServerInfrastructure(QObject *parent):
        FKInfrastructure(parent),_logged(false),_realmConnection(0),_roomModule(0),_room(0),_idgenerator(1),_id(-1){
    FK_CBEGIN
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

void FKServerInfrastructure::requestLoginRealm(const qint32 id, const QString password){
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

void FKServerInfrastructure::registerRoomTypeRequest(const QString roomType){
    if(roomType.isEmpty()){
        emit messageRequested(QString(tr("Unable register empty room type")));
        return;
    }
    if(!_logged){
        emit messageRequested(QString(tr("Unable register room type: server is not not logged in")));
        return;
    }
    if(_registeredRoomTypes.contains(roomType)){
        emit messageRequested(QString(tr("Unable register room type: already registered")));
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
    QString roomType(value.toString());
    if(!roomType.isEmpty()){
        _registeredRoomTypes.append(roomType);
        emit messageRequested(QString(tr("Room type registration success")));
    }else{
        emit messageRequested(QString(tr("Room type not registered")));
    }
}

void FKServerInfrastructure::roomTypesNotification(const QVariant& value){
    _registeredRoomTypes=value.toStringList();
}

void FKServerInfrastructure::removeRoomTypeRequest(const QString roomType){
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
    QString roomType(value.toString());
    if(!roomType.isEmpty()){
        _registeredRoomTypes.removeOne(roomType);
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
                msg=QString(tr("Failed create room of type %1")).arg(roomData.roomType());
            }else{
                msg=QString(tr("Created room of type %1")).arg(roomData.roomType());
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
    if(invite.isValid()){
        if(!_room){
            notifyRealmClientDropped(invite.client());
        }else{
            _room->addUsers(invite);
        }
    }else{
        FK_MLOG("Invalid invite client data recieved")
    }
}

void FKServerInfrastructure::syncRequest(FKServerConnectionManager* guest, const QVariant& data){
    todo;
//    bool success=false;
//    FKAusviceData ausvice(data);
//    if(!ausvice.isValid()){
//        FK_MLOG("Server guest provided invalid ausvice data")
//        dropGuestConnection(guest);
//        return;
//    }

//    if(ausvice.clientId().isEmpty()){
//        FK_MLOG("Server guest provided empty client name")
//        dropGuestConnection(guest);
//        return;
//    }
//    if(!isClientId(ausvice.clientId())){
//        FK_MLOG("Server guest provided invalid client name")
//        //dropGuestConnection(guest);
//        return;
//    }
//    if(!_roomModule){
//        FK_MLOG("Server guest attempt login with no room")
//        dropGuestConnection(guest);
//        return;
//    }
//    auto i=_clients.find(ausvice.clientId());
//    if(referent==_clients.end()){
//        auto referent=new FKClientInfrastructureReferent(ausvice.clientId(),ausvice.password());
//        connect(referent,SIGNAL(actionRequested(FKEventObject*)),_om,SLOT(externalAction(FKEventObject*)));
//        i=_clients.insert(ausvice.clientId(),referent);
//    }else if(i.value()->isConnected()){
//        emit messageRequested(QString(tr("Warning! Dublicate client login attemption: %1")).arg(ausvice.clientId()));
//        dropGuestConnection(guest);
//        return;
//    }else if(i.value()->password()!=data.password()){
//        emit messageRequested(QString(tr("Warning! Client password does not match while login attemption: %1")).arg(ausvice.clientId()));
//        dropGuestConnection(guest);
//        return;
//    }
//    i.value()->setConnected(guest->connector());
//    _guests.remove(guest);
//    guest->deleteLater();
    //    syncClient(i.value());
}

void FKServerInfrastructure::stopGuestConnection(FKServerConnectionManager *guest){
    todo;
}

void FKServerInfrastructure::messageFromRealm(const QString& msg){
    emit messageRequested(QString(tr("Realm->%1")).arg(msg));
}

qint32 FKServerInfrastructure::serverId() const{
    return _id;
}

void FKServerInfrastructure::realmConnection(FKConnector* connector){
    connector->setParent(this);
    _realmConnection=new FKServerConnectionManagerR(this,connector,this);
    connect(_realmConnection,SIGNAL(connectionStatusChanged()),SLOT(realmConnectorStatusChanged()));
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::connect)){
        FK_MLOG("Unexpected behaivour in FKServerInfrastructure::realmConnection()")
    }
}

void FKServerInfrastructure::clientConnection(FKConnector* connector){
    connector->setParent(this);
    auto* mgr=new FKServerConnectionManager(this,connector,this);
    _guests.insert(mgr);
    emit messageRequested(QString(tr("Server got new income connection")));
}

bool FKServerInfrastructure::waitingForAnswer()const{
    return FKInfrastructure::waitingForAnswer(FKInfrastructureType::Realm);
}

bool FKServerInfrastructure::isLogged() const{
    return _logged;
}

void FKServerInfrastructure::setPort(const qint32 clientsPort){
    _clientsPort=clientsPort;
}

void FKServerInfrastructure::setRealmConnectionSettings(const QString ip, const qint32 port){
    _realmIP=ip;
    _realmPort=port;
}

QString FKServerInfrastructure::realmIP() const{
    return _realmIP;
}

qint32 FKServerInfrastructure::realmPort() const{
    return _realmPort;
}

qint32 FKServerInfrastructure::clientPort() const{
    return _clientsPort;
}

QString FKServerInfrastructure::serverIP() const{
    return QString();
}

QStringList FKServerInfrastructure::registeredRoomTypes() const{
    return _registeredRoomTypes;
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

void FKServerInfrastructure::roomDataChanged(const qint32 propName, const QVariant value){
    if(!_logged){
        FK_MLOG("Warning! Unable send room data delta to realm: server is not logged in")
        return;
    }

    QVariant delta=FKRoomData::createDelta(propName,value);
    if(delta.isValid()){
        FKBasicEvent ev(FKBasicEventSubject::roomData,delta);
        _realmConnection->sendBasicEvent(&ev);
    }
}

//void FKServerInfrastructure::clientInviteResolved(const FKRoomInviteData data, const QList<qint32> userObjects){
//    if(userObjects.isEmpty()){
//        notifyRealmClientDropped(data.client());
//    }else{
//        auto i=_clients.find(data.client());
//        if(i==_clients.end()){
//            auto referent=new FKClientInfrastructureReferent(data.client(),data.password());
//            connect(referent,SIGNAL(actionRequested(FKEventObject*)),_om,SLOT(externalAction(FKEventObject*)));
//            i=_clients.insert(data.client(),referent);
//        }else if(i.value()->isInvited()){
//            emit messageRequested(QString(tr("Warning! Dublicate client invite resolve attemption: %1")).arg(data.client()));
//            return;
//        }else if(i.value()->password()!=data.password()){
//            i.value()->dropConnection();
//            i.value()->setPassword(data.password());
//        }
//        const qint32 listSize=userObjects.size();
//        QStringList userNames(data.users());
//        for(qint32 ind=0;ind<listSize;++ind){
//            i.value()->addUser(userObjects.at(ind));
//            _users.insert(userNames.at(ind),userObjects.at(ind));
//            _referents.insert(userObjects.at(ind),i.value());
//        }
//        i.value()->setInvited();
//        syncClient(i.value());
//    }
//}

//void FKServerInfrastructure::roomStarted(){
//    if(!_logged){
//        FK_MLOG("Warning! Unable send room started event to realm: server is not logged in")
//        return;
//    }
//    FKBasicEvent ev(FKBasicEventSubject::startRoom);
//    _realmConnection->sendBasicEvent(&ev);
//}

void FKServerInfrastructure::roomStopped(){
    if(!_logged){
        FK_MLOG("Warning! Unable send room stopped event to realm: server is not logged in")
        return;
    }
    FKBasicEvent ev(FKBasicEventSubject::stopRoom);
    _realmConnection->sendBasicEvent(&ev);
}

void FKServerInfrastructure::dispatchEvent(FKEventObject* ev){
    QList<FKClientInfrastructureReferent*> recievers;
    for(auto i=ev->recievers().constBegin();i!=ev->recievers().constEnd();++i){
        auto referent=_referents.find(*i);
        if(referent!=_referents.constEnd()){
            recievers.append(referent.value());
        }else{
            emit messageRequested(QString(tr("Unable dispatch event: %1 user not found")).arg(QString::number(*i)));
        }
    }
    for(auto r=recievers.constBegin();r!=recievers.constEnd();++r){
        (*r)->pushEvent(ev);
    }
    for(auto r=recievers.constBegin();r!=recievers.constEnd();++r){
        (*r)->sendEvent();
    }
    ev->deleteLater();
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
        _roomModule=new FKRoomModule(this,roomData.roomType());
        if(_roomModule->load()){
            FKObject* room=_om->genObject(_roomModule->roomClass());
            _room=qobject_cast<FKRoom*>(room);
            if(!_room){
                FK_MLOGV("Invalid room module",roomData.roomType())
                if(room)_om->deleteObject(room);
                _roomModule->deleteLater();
                _roomModule=nullptr;
            }else{
                //_om->setRoomModule(_roomModule);
                connect(_room,SIGNAL(roomDataChanged(qint32,QVariant)),SLOT(roomDataChanged(qint32,QVariant)));
                connect(_room,SIGNAL(clientInviteResolved(FKRoomInviteData,QList<qint32>)),SLOT(clientInviteResolved(FKRoomInviteData,QList<qint32>)));
                _room->setup(roomData);
                FK_MLOG("room created on server")
                answer=true;
            }
        }else{
            FK_MLOGV("Unable load room module",roomData.roomType())
            _roomModule->deleteLater();
            _roomModule=nullptr;
        }
    }else{
        FK_MLOG("room module is not empty")
    }
    return answer;
}

void FKServerInfrastructure::syncClient(FKClientInfrastructureReferent* client){
    if(client->isActive()){
        FKBasicEvent startSync(FKBasicEventSubject::login,QVariant::fromValue(client->users()));
        client->sendBasicEvent(&startSync);
        _room->enableUsers(client->users());
        FKBasicEvent finishSync(FKBasicEventSubject::sync);
        client->sendBasicEvent(&finishSync);
    }
}

void FKServerInfrastructure::dropClient(const QString& clientName){
    todo; //remove from maps
    notifyRealmClientDropped(clientName);
}

void FKServerInfrastructure::notifyRealmClientDropped(const QString& clientName){
    FKBasicEvent ev(FKBasicEventSubject::dropClient,clientName);
    _realmConnection->sendBasicEvent(&ev);
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


