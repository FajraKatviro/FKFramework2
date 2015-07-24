#include "FKRealm.h"

#include "FKRealmConnectionManager.h"
#include "FKDataBase.h"
#include "FKBasicEvent.h"
#include "FKRoomDataFilter.h"

#include "FKAusviceData.h"
#include "FKRoomInviteData.h"
#include "FKBasicEventSubjects.h"
#include "FKLogger.h"

/*!
\class FKRealm
\brief This class represents realm infrastructure. Realm manages multiple other infrastructures and make login validation.
Realm has own database for information storing. Structure of the db is following:

root
|
|\
| serverBranch
| |
|  \
|   id* - password
|   |
|   |\
|   | currentRoom - id
|   |
|   |\
|   | pendingClient - customClientId
|   |
|    \
|     roomsNode
|     |
|      \
|       roomType*
|
|\
| clientBranch
| |
|  \
|   client* - password
|   |
|   |\
|   | usersNode
|   | |
|   |  \
|   |   user* - userStatus
|   |
|   |\
|   | roomNode - roomId
|   |
|    \
|     customServer - serverId
|
|\
| userBranch
| |
|  \
|   user* - clientId
|
 \
  roomBranch
    |
     \
      roomType*
      |
       \
        avaliableServers*
*/

/*!
 * \brief Creates realm object with given parent
 */

FKRealm::FKRealm(QObject *parent):FKInfrastructure(parent){
    FK_CBEGIN
    qsrand(QTime::currentTime().msec());
    FK_CEND
}

/*!
 * \brief Deletes realm object
 */

FKRealm::~FKRealm(){
    FK_DBEGIN
    FK_DEND
}

/*!
 * \brief Process guest login request. Validate income data and replace guest manager with client or server manager on success.
 */

void FKRealm::ausvise(FKConnectionManager* source, const QVariant loginData){
    if(!source){
        FK_MLOG("Empty source connection manager passed to realm ausvice")
        return;
    }
    
    FKAusviceData data(loginData);
    if(!data.isValid()){
        FK_MLOGV("Guest ausvice error",data.error())
        stopGuestConnection(source);
        return;
    }
    
    if(data.isClient()){
        if(isClientConnected(data.clientId())){
            FK_MLOGV("Requested client already connected",data.clientId())
            stopGuestConnection(source);
            return;
        }

        const QString clientPassword=getClientPassword(data.clientId());
        if(clientPassword.isEmpty()){
            FK_MLOGV("Client id provided by guest is not registered",data.clientId())
            FKBasicEvent deny(FKBasicEventSubject::login,false);
            source->sendBasicEvent(&deny);
            const QString message(tr("Invalid client id"));
            source->sendMessage(message);
            return;
        }

        if(clientPassword!=data.password()){
            FK_MLOG("Client password provided by guest does not match")
            FKBasicEvent deny(FKBasicEventSubject::login,false);
            source->sendBasicEvent(&deny);
            const QString message(tr("Invalid client password"));
            source->sendMessage(message);
            return;
        }

        incomeClient(data.clientId(),source->connector());

    }else{
        if(isServerConnected(data.serverId())){
            FK_MLOGV("Requested server already started",data.serverId())
            stopGuestConnection(source);
            return;
        }

        const QString serverPassword=getServerPassword(data.serverId());
        if(serverPassword.isEmpty()){

            FK_MLOGV("Server id provided by guest is not registered",data.serverId())
            FKBasicEvent deny(FKBasicEventSubject::login,false);
            source->sendBasicEvent(&deny);
            const QString message(tr("Invalid server id"));
            source->sendMessage(message);
            return;
        }

        if(serverPassword!=data.password()){
            emit messageRequested(QString(tr("Server password provided by guest does not match for server %1")).arg(data.serverId()));
            FKBasicEvent deny(FKBasicEventSubject::login,false);
            source->sendBasicEvent(&deny);
            const QString message(tr("Invalid server password"));
            source->sendMessage(message);
            return;
        }

        incomeServer(data.serverId(),source->connector());
    }
    _guesConnections.remove(source);
    source->deleteLater();
}

/*!
 * \brief Drop guest connection, remove manager and delete it
 */

void FKRealm::stopGuestConnection(FKConnectionManager* guest){
    if(_guesConnections.remove(guest)){
        guest->dropConnection();
        guest->deleteLater();
        emit messageRequested(QString(tr("Guest connection stopped")));
    }else{
        FK_MLOG("Failed stop guest connection")
    }
}

/*!
 * \brief Drop server connection, remove manager and delete it
 */

void FKRealm::stopServerConnection(const qint32 serverId){
    FKConnectionManager* server=_serverConnections.value(serverId,0);
    if(server){
        roomStopped(serverId);
        _serverConnections.remove(serverId);
        server->dropConnection();
        server->deleteLater();
        emit messageRequested(QString(tr("Server %1 connection stopped")).arg(QString(serverId)));
    }else{
        FK_MLOGV("Failed stop server connection (not found)",serverId)
    }
}

/*!
 * \brief Drop client connection, remove manager and delete it
 */

void FKRealm::stopClientConnection(const QString& clientId){
    FKConnectionManager* client=_clientConnections.value(clientId,0);
    if(client){
        _clientConnections.remove(clientId);
        client->dropConnection();
        client->deleteLater();
        emit messageRequested(QString(tr("Client %1 connection stopped")).arg(clientId));
    }else{
        FK_MLOGV("Failed stop client connection (not found)",clientId)
    }
}

/*!
 * \brief Respond client request: create list of avaliable rooms, apply \i filters and send to client.
 */

void FKRealm::refreshRoomList(const QString& clientId, const QVariant& filters){
    auto rooms=getRoomList(filters);
    FKConnectionManager* mgr=_clientConnections.value(clientId);
    FKBasicEvent ev(FKBasicEventSubject::roomList,rooms);
    mgr->sendBasicEvent(&ev);
    emit messageRequested(QString(tr("Room list sent to %1 client")).arg(clientId));
}

/*!
 * \brief Respond server request: update server's room info
 */

void FKRealm::refreshRoomData(const qint32 serverId, const QVariant& data){
    QString roomId=getServerRoom(serverId);
    if(roomId.isEmpty()){
        emit messageRequested(QString(tr("Unexpected room data change from %1 server (no room founded)")).arg(QString::number(serverId)));
        return;
    }
    _activeRooms[roomId].change(data);
}

/*!
 * \brief Disconnect all connected infrastructures
 */

void FKRealm::dropRealm(){
    foreach(FKConnectionManager* guest,_guesConnections){
        stopGuestConnection(guest);
    }
    foreach(QString client,_clientConnections.keys()){
        stopClientConnection(client);
    }
    foreach(qint32 server,_serverConnections.keys()){
        stopServerConnection(server);
    }
}

/*!
 * \brief This is reimplemented function
 */

FKInfrastructureType FKRealm::infrastructureType() const{
    return FKInfrastructureType::Realm;
}

/*!
 * \brief Try create new client record with given parametrs
 */

void FKRealm::createClientRecord(const QString& clientName, const QString& password){
    if(password.isEmpty()){
        emit messageRequested(QString(tr("Unable create client: empty password provided")));
        return;
    }
    if(clientName.isEmpty()){
        emit messageRequested(QString(tr("Unable create client: empty name provided")));
        return;
    }
    if(!isClientId(clientName)){
        emit messageRequested(QString(tr("Unable create client: invalid name provided")));
        return;
    }
    if(isClientExists(clientName)){
        emit messageRequested(QString(tr("Unable create client: name already in use")));
        return;
    }

    createNewClientRecord(clientName,password);
    emit messageRequested(QString(tr("Client record creation success")));
}

/*!
 * \brief Try create new record for server with given password. Returns server id on success or -1 on fail.
 */

qint32 FKRealm::createServerRecord(const QString& password){
    if(password.isEmpty()){
        emit messageRequested(QString(tr("Unable create server record: empty password provided")));
        return -1;
    }
    qint32 id=createNewServerRecord(password);
    emit messageRequested(QString(tr("Server record %1 created")).arg(QString::number(id)));
    return id;
}

/*!
 * \brief Register room type to allow request rooms of that type.
 */

void FKRealm::registerRoomType(const QString& roomType){
    QString error;
    if(roomType.isEmpty()){
        error=QString(tr("Unable register room type: empty type"));
    }else if(!isRoomTypeName(roomType)){
        error=QString(tr("Unable register room type: invalid type"));
    }else if(isRoomTypeRegistered(roomType)){
        error=QString(tr("Unable register room type: %1 already registered")).arg(roomType);
    }
    if(error.isEmpty()){
        registerNewRoomType(roomType);
        emit messageRequested(QString(tr("Room type %1 registered")).arg(roomType));
    }else{
        emit messageRequested(error);
    }
}

void FKRealm::createRoomRealmRequest(const QString& roomName, const QString& roomType){
    QString error;
    if(roomType.isEmpty()){
        error=QString(tr("Unable create room: empty type"));
    }else if(!isRoomTypeName(roomType)){
        error=QString(tr("Unable create room: invalid type"));
    }else if(!isRoomTypeRegistered(roomType)){
        error=QString(tr("Unable create room: type %1 not registered")).arg(roomType);
    }else if(roomName.isEmpty()){
        error=QString(tr("Unable create room: empty name"));
    }else if(!isRoomName(roomName)){
        error=QString(tr("Unable create room: invalid name"));
    }else if(isRoomExists(roomName)){
        error=QString(tr("Unable create room: name already in use"));
    }

    if(error.isEmpty())error=createNewRoomForServer(roomName,roomType);
    if(!error.isEmpty())emit messageRequested(error);
}

void FKRealm::registerServerRoomType(const qint32 serverId, const QVariant& data){
    QString error, roomType(data.toString());
    if(roomType.isEmpty()){
        error=QString(tr("Unable register room type: empty type"));
    }else if(!isRoomTypeName(roomType)){
        error=QString(tr("Unable register room type: invalid type"));
    }else if(!isRoomTypeRegistered(roomType)){
        error=QString(tr("Unable register room type: room type not registered at realm")).arg(roomType);
    }else if(isRoomTypeRegistered(roomType,serverId)){
        error=QString(tr("Unable register room type: %1 already registered")).arg(roomType);
    }
    FKConnectionManager* connector=_serverConnections.value(serverId);
    bool result;
    if(error.isEmpty()){
        registerNewRoomType(serverId,roomType);
        result=true;
    }else{
        connector->sendMessage(error);
        result=false;
    }
    FKBasicEvent answer(FKBasicEventSubject::registerRoomType,result);
    connector->sendBasicEvent(&answer);
}

/*!
 * \brief Try create user for given client and send reply
 */

void FKRealm::createUser(const QString& clientId, const QVariant& userName){
    QString errorMessage;
    QString name=userName.toString();
    if(getClientRoomState(clientId)!=FKClientRoomState::NotInRoom){
        errorMessage=QString(tr("Unable create user: invalid client state"));
    }else if(name.isEmpty()){
        errorMessage=QString(tr("Unable create user: invalid argument"));
    }else if(!isUserName(name)){
        errorMessage=QString(tr("Unable create user: invalid username"));
    }else if(isUserExists(name)){
        errorMessage=QString(tr("Unable create user: username already in use"));
    }

    FKConnectionManager* mgr=_clientConnections.value(clientId);
    if(errorMessage.isEmpty()){
        createUser(clientId,name);
    }else{
        name.clear();
        mgr->sendMessage(errorMessage);
    }
    FKBasicEvent ev(FKBasicEventSubject::createUser,name);
    mgr->sendBasicEvent(&ev);
}

/*!
 * \brief Try delete user for given client and send reply
 */

void FKRealm::deleteUser(const QString& clientId, const QVariant& userName){
    QString errorMessage;
    QString name=userName.toString();
    if(getClientRoomState(clientId)!=FKClientRoomState::NotInRoom){
        errorMessage=QString(tr("Unable delete user: invalid client state"));
    }else if(name.isEmpty()){
        errorMessage=QString(tr("Unable delete user: invalid argument"));
    }else if(!isUserName(name)){
        errorMessage=QString(tr("Unable delete user: invalid username"));
    }else if(!isUserExists(name,clientId)){
        errorMessage=QString(tr("Unable delete user: no such user found"));
    }else if(getUserActive(clientId,name)){
        errorMessage=QString(tr("Unable delete user. Please check user not active"));
    }

    FKConnectionManager* mgr=_clientConnections.value(clientId);
    if(errorMessage.isEmpty()){
        deleteUser(clientId,name);
    }else{
        name.clear();
        mgr->sendMessage(errorMessage);
    }
    FKBasicEvent ev(FKBasicEventSubject::deleteUser,name);
    mgr->sendBasicEvent(&ev);
}

/*!
 * \brief Try select given user for given client
 */

void FKRealm::selectUser(const QString& clientId, const QVariant& userName){
    QString errorMessage;
    QString name=userName.toString();
    if(getClientRoomState(clientId)!=FKClientRoomState::NotInRoom){
        errorMessage=QString(tr("Unable select user: invalid client state"));
    }else if(name.isEmpty()){
        errorMessage=QString(tr("Unable select user: invalid argument"));
    }else if(!isUserName(name)){
        errorMessage=QString(tr("Unable select user: invalid username"));
    }else if(!isUserExists(name,clientId)){
        errorMessage=QString(tr("Unable select user: no such user found"));
    }else if(getUserActive(clientId,name)){
        errorMessage=QString(tr("Unable select user. Please check user not active"));
    }

    FKConnectionManager* mgr=_clientConnections.value(clientId);
    if(errorMessage.isEmpty()){
        selectUser(clientId,name);
    }else{
        name.clear();
        mgr->sendMessage(errorMessage);
    }
    FKBasicEvent ev(FKBasicEventSubject::selectUser,name);
    mgr->sendBasicEvent(&ev);
}

/*!
 * \brief Try deselect given user for given client
 */

void FKRealm::deselectUser(const QString& clientId, const QVariant& userName){
    QString errorMessage;
    QString name=userName.toString();
    if(getClientRoomState(clientId)!=FKClientRoomState::NotInRoom){
        errorMessage=QString(tr("Unable deselect user: invalid client state"));
    }else if(name.isEmpty()){
        errorMessage=QString(tr("Unable deselect user: invalid argument"));
    }else if(!isUserName(name)){
        errorMessage=QString(tr("Unable deselect user: invalid username"));
    }else if(!isUserExists(name,clientId)){
        errorMessage=QString(tr("Unable deselect user: no such user found"));
    }else if(!getUserActive(clientId,name)){
        errorMessage=QString(tr("Unable deselect user. Please check user is active"));
    }

    FKConnectionManager* mgr=_clientConnections.value(clientId);
    if(errorMessage.isEmpty()){
        deselectUser(clientId,name);
    }else{
        name.clear();
        mgr->sendMessage(errorMessage);
    }
    FKBasicEvent ev(FKBasicEventSubject::deselectUser,name);
    mgr->sendBasicEvent(&ev);
}

/*!
 * \brief Proccess clients request to start custom server
 */

void FKRealm::customServerRequested(const QString& clientId, const QVariant& data){
    QString error;
    if(data.isValid()){
        error=QString(tr("Custom server request declined: unexpected data provided"));
    }else if(getClientRoomState(clientId)!=FKClientRoomState::NotInRoom){
        error=QString(tr("Custom server request declined: client is in room"));
    }else if(isCustomServerRequested(clientId)){
        error=QString(tr("Custom server request declined: already requested"));
    }

    FKConnectionManager* mgr=_clientConnections.value(clientId);
    QVariant ret;
    if(error.isEmpty()){
        ret=customServerPreserve(clientId).toVariant();
    }else{
        ret=FKAusviceData().toVariant();
        mgr->sendMessage(error);
    }
    FKBasicEvent ev(FKBasicEventSubject::customServer,ret);
    mgr->sendBasicEvent(&ev);
}

void FKRealm::customServerRejected(const QString& clientId, const QVariant& data){
    todoNow;
}

/*!
 * \brief Proccess clients request to start new room
 */

void FKRealm::createRoomRequested(const QString& clientId, const QVariant& data){
    FKRoomCreateData roomData(data);
    QString error, roomName(roomData.roomId()), roomType(roomData.roomType());
    if(getClientRoomState(clientId)!=FKClientRoomState::NotInRoom){
        error=QString(tr("Unable select user: invalid client state"));
    }else if(roomName.isEmpty()){
        error=QString(tr("Room request declined: empty room name provided"));
    }else if(roomType.isEmpty()){
        error=QString(tr("Room request declined: empty room type provided"));
    }else if(!isRoomName(roomName)){
        error=QString(tr("Room request declined: invalid room name provided"));
    }else if(!isRoomTypeName(roomType)){
        error=QString(tr("Room request declined: invalid room type provided"));
    }else if(!isRoomTypeRegistered(roomType)){
        error=QString(tr("Room request declined: room type not registered"));
    }else if(isRoomExists(roomName)){
        error=QString(tr("Room request declined: room name already in use"));
    }else if(!hasSelectedUser(clientId)){
        error=QString(tr("Room request declined: no users selected"));
    }else if(getClientRoomState(clientId)!=FKClientRoomState::NotInRoom){
        error=QString(tr("Room request declined: client already in room"));
    }

    if(error.isEmpty()){
       error=createNewRoomForServer(roomName,roomType,clientId);
    }

    if(!error.isEmpty()){
       FKConnectionManager* clientMgr=_clientConnections.value(clientId);
       clientMgr->sendMessage(error);
       FKBasicEvent ev(FKBasicEventSubject::createRoom);
       clientMgr->sendBasicEvent(&ev);
    }else{
        setClientRoomState(clientId,FKClientRoomState::CreateRoomRequested);
    }
}

QString FKRealm::createNewRoomForServer(const QString& roomName,const QString& roomType,const QString& owner){
    FKConnectionManager* mgr=0;
    qint32 serverId;
    bool custom=false;
    qint32 customServer=getCustomServer(owner);
    if(customServer>0){
        serverId=customServer;
        custom=true;
        mgr=_serverConnections.value(serverId,0);
        if(!mgr)return QString(tr("Unable create custom room: server not found"));
    }else{
        serverId=getFreeServer(roomType);
        mgr=_serverConnections.value(serverId,0);
        if(!mgr)return QString(tr("Unable create room: no avaliable servers"));
    }

    QVariant outData=setServerRoomData(serverId,roomName,roomType,owner,custom).toVariant();
    emit messageRequested(QString(tr("Room %1 of type %2 creation request sent to %3 server")).arg(roomName).arg(roomType).arg(QString::number(serverId)));
    FKBasicEvent ev(FKBasicEventSubject::createRoom,outData);
    mgr->sendBasicEvent(&ev);
    return QString();
}

void FKRealm::enterRoomRequested(const QString& clientId, const QVariant& data){
    const QString roomId=data.toString();
    QString error;
    if(roomId.isEmpty()){
        error=QString(tr("Invalid enter room request: empty roomName"));
    }else if(!isRoomName(roomId)){
        error=QString(tr("Invalid enter room request: invalid roomName"));
    }else if(!isRoomExists(roomId)){
        error=QString(tr("Invalid enter room request: room does not exists"));
    }else if(getClientRoomState(clientId)!=FKClientRoomState::NotInRoom){
        error=QString(tr("Unable enter room: invalid client room state"));
    }

    if(error.isEmpty()){
        enterRoomRequested(clientId,roomId);
    }else{
        auto mgr=_clientConnections[clientId];
        mgr->sendMessage(error);
        FKBasicEvent fail(FKBasicEventSubject::joinRoom,false);
        mgr->sendBasicEvent(&fail);
    }
}

void FKRealm::createRoomRespond(const qint32 serverId, const QVariant& data){
    QString room=getServerRoom(serverId);
    if(room.isEmpty()){
        emit messageRequested(QString(tr("Unexpected room creation respond from %1 server")).arg(QString::number(serverId)));
        return;
    }
    bool success=data==QVariant(true);
    QString client=_activeRooms[room].owner();
    if(!client.isEmpty()){
        if(getClientRoomState(client)!=FKClientRoomState::CreateRoomRequested){
            emit messageRequested(QString(tr("Invalid client room state when room created on %1 server")).arg(QString::number(serverId)));
        }else{
            setClientRoomState(client,FKClientRoomState::NotInRoom);
        }
    }
    FKConnectionManager* mgr=_clientConnections.value(client,0);

    if(success){
        submitRoomData(room);
    }else{
        abortRoomData(serverId,room);
    }

    if(!client.isEmpty()){
        if(mgr){
            FKBasicEvent creation(FKBasicEventSubject::createRoom,success);
            mgr->sendBasicEvent(&creation);
            if(success){
                enterRoomRequested(client,room);
            }
        }else{
            emit messageRequested(QString(tr("Unable respond creating room client %1: connection not found")).arg(client));
        }
    }
}

void FKRealm::enterRoomRespond(const qint32 serverId, const QVariant& data){
    QString room=getServerRoom(serverId);
    FKRoomInviteData invite(data);
    QString error;
    if(room.isEmpty()){
        error=QString(tr("Unexpected room enter respond from %1 server")).arg(QString::number(serverId));
    }else if(invite.client().isEmpty()){
        error=QString(tr("Invalid room enter respond from %1 server: no client provided")).arg(QString::number(serverId));
    }else if(!isClientId(invite.client())){
        error=QString(tr("Invalid room enter respond from %1 server: invalid client id")).arg(QString::number(serverId));
    }else if(!isClientExists(invite.client())){
        error=QString(tr("Invalid room enter respond from %1 server: no client provided")).arg(QString::number(serverId));
    }else if(getClientRoomState(invite.client())!=FKClientRoomState::EnterRoomRequested){
        error=QString(tr("Invalid room enter respond from %1 server: wrong client state (%2)")).arg(QString::number(serverId)).arg(QString::number((qint32)getClientRoomState(invite.client())));
    }else if(getClientRoom(invite.client())!=room){
        error=QString(tr("Invalid room enter respond from %1 server: client room miss-match")).arg(QString::number(serverId));
    }

    if(!error.isEmpty()){
        emit messageRequested(error);
        return;
    }

    QVariant ret;
    bool success=false;
    FKConnectionManager* mgr=_clientConnections.value(invite.client(),0);
    if(mgr){
        if(invite.isValid()){
            if(processInviteData(invite)){
                setClientRoomState(invite.client(),FKClientRoomState::Invited);
                invite.setAddress(getServerIP(serverId));
                ret=invite.toVariant();
                success=true;
            }
        }
    }
    if(!success){
        removeClientFromRoom(serverId,invite.client());
        FKRoomInviteData fail;
        ret=fail.toVariant();
    }
    if(mgr){
        FKBasicEvent answer(FKBasicEventSubject::joinRoom,ret);
        mgr->sendBasicEvent(&answer);
    }else{
        emit messageRequested(QString(tr("Unable reply enter room to client %1: connection not found")).arg(invite.client()));
    }
}

void FKRealm::roomStarted(const qint32 serverId, const QVariant& data){
    Q_UNUSED(data)
    QString room=getServerRoom(serverId);
    auto i=_activeRooms.find(room);
    if(i!=_activeRooms.end()){
        database()->writeValue(FKDBValue(true),_dbPath.serverRoomStartedIndex(serverId),false);
        _activeRooms.erase(i);
    }else{
        emit messageRequested(QString(tr("Unexpected room started event from %1 server: no room found")).arg(QString::number(serverId)));
    }
}

void FKRealm::roomStopped(const qint32 serverId, const QVariant& data){
    Q_UNUSED(data)
    roomStopped(serverId);
}

void FKRealm::roomStopped(const qint32 serverId){
    FKDBIndex roomStartedInd=_dbPath.serverRoomStartedIndex(serverId);
    bool wasStarted=database()->getValue(roomStartedInd,false).boolean();
    if(wasStarted){
        database()->removeIndex(roomStartedInd);
    }else{
        QString room=getServerRoom(serverId);
        auto i=_activeRooms.find(room);
        if(i!=_activeRooms.end()){
            _activeRooms.erase(i);
        }else{
            emit messageRequested(QString(tr("Unable stop room for %1 server: no room found")).arg(QString::number(serverId)));
        }
    }
    database()->removeIndex(_dbPath.serverRoomIndex(serverId));
    //todo();//remove clients from room
}

/*!
 * \brief Return list of users registered in database
 */

QStringList FKRealm::userList() const{
    return database()->getProperties(_dbPath.usersIndex());
}

void FKRealm::createUser(const QString& clientId, const QString& userName){
    database()->writeValue(FKDBValue(false),_dbPath.clientUserIndex(clientId,userName),false);
    database()->writeValue(FKDBValue(clientId),_dbPath.userIndex(userName),false);
    emit messageRequested(QString(tr("New user %2 created for %1 client")).arg(clientId).arg(userName));
}

void FKRealm::deleteUser(const QString& clientId, const QString& userName){
    database()->removeIndex(_dbPath.clientUserIndex(clientId,userName));
    database()->removeIndex(_dbPath.userIndex(userName));
    emit messageRequested(QString(tr("User %2 deleted for %1 client")).arg(clientId).arg(userName));
}

void FKRealm::selectUser(const QString& clientId, const QString& userName){
    database()->writeValue(FKDBValue(true),_dbPath.clientUserIndex(clientId,userName),false);
    emit messageRequested(QString(tr("User %2 selected for %1 client")).arg(clientId).arg(userName));
}

void FKRealm::deselectUser(const QString& clientId, const QString& userName){
    database()->writeValue(FKDBValue(false),_dbPath.clientUserIndex(clientId,userName),false);
    emit messageRequested(QString(tr("User %2 deselected for %1 client")).arg(clientId).arg(userName));
}

void FKRealm::createNewClientRecord(const QString& clientName, const QString& password){
    database()->writeValue(FKDBValue(password),_dbPath.clientIndex(clientName),false);
    setClientRoomState(clientName,FKClientRoomState::NotInRoom);
}

qint32 FKRealm::createNewServerRecord(const QString& password){
    qint32 id=nextServerId();
    database()->writeValue(FKDBValue(password),_dbPath.serverIndex(id),false);
    return id;
}

qint32 FKRealm::getFreeServer(const QString& roomType) const{
    FKDBIndex ind=database()->findIndex(FKDBValue(),_dbPath.roomTypeIndex(roomType),true);
    if(ind.isValid()){
        return ind.property().toInt();
    }else{
        return 0;
    }
}

void FKRealm::registerNewRoomType(const QString& roomType){
    database()->addIndex(_dbPath.roomTypeIndex(roomType));
}

void FKRealm::registerNewRoomType(const qint32 serverId, const QString& roomType){
    database()->addIndex(_dbPath.serverRoomTypeIndex(serverId,roomType));
    addServerToRoomType(serverId,roomType);
}

void FKRealm::addServerToRoomType(const qint32 serverId, const QString& roomType){
    database()->writeValue(FKDBValue(_dbPath.serverRoomIndex(serverId)),_dbPath.roomTypeServerIndex(roomType,serverId),false);
}

QStringList FKRealm::getRoomList(const QVariant& filters) const{
    QStringList lst;
    FKRoomDataFilter f(filters);
    for(auto i=_activeRooms.constBegin();i!=_activeRooms.constEnd();++i){
        if(i.value().fit(f)){
            lst.append(i.key());
        }
    }
    return lst;
}

QString FKRealm::getServerRoom(const qint32 serverId) const{
    return database()->getValue(_dbPath.serverRoomIndex(serverId),false).string();
}

QString FKRealm::getClientRoom(const QString& clientId) const{
    return database()->getValue(_dbPath.clientRoomIndex(clientId),false).string();
}

qint32 FKRealm::getUserActive(const QString& clientId, const QString& userName){
    return database()->getValue(_dbPath.clientUserIndex(clientId,userName)).boolean();
}

bool FKRealm::hasSelectedUser(const QString& clientId) const{
    return database()->findIndex(FKDBValue(true),_dbPath.clientUsersIndex(clientId)).isValid();
}

qint32 FKRealm::nextServerId() const{
    return database()->countValues(_dbPath.serversIndex())+1;
}

FKClientRoomState FKRealm::getClientRoomState(const QString& clientId) const{
    return FKClientRoomState(database()->getValue(_dbPath.clientRoomStateIndex(clientId),false).number());
}

void FKRealm::setClientRoomState(const QString& clientId, FKClientRoomState state) const{
    database()->writeValue(FKDBValue(qreal(state)),_dbPath.clientRoomStateIndex(clientId),false);
}

bool FKRealm::isCustomServerRequested(const QString& clientId) const{
    return _customServerRequestedClients.contains(clientId);
}

qint32 FKRealm::getCustomServer(const QString& clientId) const{
    if(clientId.isEmpty())return 0;
    return database()->getValue(_dbPath.clientCustomServerIndex(clientId),false).number();
}

QString FKRealm::getServerIP(const qint32 serverId){
    QString ip;
    FKConnectionManager* server=_serverConnections.value(serverId,0);
    if(server){
        ip=server->address();
    }
    return ip;
}

FKAusviceData FKRealm::customServerPreserve(const QString& clientId){
    qint32 serverId;
    QString serverPassword;

    FKDBIndex ind=_dbPath.clientCustomServerIndex(clientId);
    if(database()->hasIndex(ind)){
        serverId=database()->getValue(ind,false).number();
        serverPassword=database()->getValue(_dbPath.serverIndex(serverId),false).string();
    }else{
        serverPassword=generateServerPassword();
        serverId=createServerRecord(serverPassword);
        database()->writeValue(FKDBValue(qreal(serverId)),ind,false);
    }
    _customServerRequestedClients.insert(clientId);
    database()->writeValue(FKDBValue(clientId),_dbPath.serverOwnerIndex(serverId),false);
    return FKAusviceData(serverId,serverPassword);
}

bool FKRealm::processInviteData(const FKRoomInviteData& invite){
    QStringList activeUsers(getActiveUsers(invite.client()));
    QMap<QString,QString> inviteUsers(invite.passwords());
    if(inviteUsers.size()!=activeUsers.size()){
        emit messageRequested(QString(tr("Invalid invite request from server: user list size does not match")));
        return false;
    }
    for(auto i=inviteUsers.constBegin();i!=inviteUsers.constEnd();++i){
        if(activeUsers.removeAll(i.key())!=1){
            emit messageRequested(QString(tr("Invalid invite request from server: user list does not match")));
            return false;
        }
        if(i.value().isEmpty()){
            emit messageRequested(QString(tr("Invalid invite request from server: empty password provided")));
            return false;
        }
    }
    return true;
}

QString FKRealm::generateServerPassword(){
    return createRandomString(10,20);
}

const FKRoomData& FKRealm::setServerRoomData(const qint32 serverId, const QString& roomName, const QString& roomType, const QString& clientId, const bool custom){
    database()->writeValue(FKDBValue(roomName),_dbPath.serverRoomIndex(serverId),false);
    return _activeRooms.insert(roomName,FKRoomData(serverId,roomType,clientId,QDateTime::currentDateTime(),custom)).value();
}

void FKRealm::submitRoomData(const QString& roomId){
    _activeRooms[roomId].setReady(true);
    emit messageRequested(QString(tr("Room creation submitted: %1")).arg(roomId));
}

void FKRealm::abortRoomData(const qint32 serverId, const QString& roomId){
    _activeRooms.remove(roomId);
    database()->setValue(FKDBValue(),_dbPath.serverRoomIndex(serverId),false);
    emit messageRequested(QString(tr("Room creation aborted: %1")).arg(roomId));
}

void FKRealm::enterRoomRequested(const QString& clientId, const QString& roomId){
    auto i=_activeRooms.constFind(roomId);
    QString error;
    if(i==_activeRooms.constEnd()){
        error=QString(tr("Unable join room %1: room not found")).arg(roomId);
    }else{
        const FKRoomData& roomData=i.value();
        QStringList activeUsers(getActiveUsers(clientId));
        qint32 newUsersCount=activeUsers.count();
        if(roomData.users()+newUsersCount>roomData.maximumUsers()){
            error=QString(tr("Unable join room %1: not enough user slots")).arg(roomId);
        }else{
            FKConnectionManager* server=_serverConnections.value(roomData.server(),0);
            if(!server){
                error=QString(tr("Unable join room %1: server not found")).arg(roomId);
            }else{
                addClientToRoom(roomData.server(),clientId,roomId); //this call to remove client from room on room stopped later
                setClientRoomState(clientId,FKClientRoomState::EnterRoomRequested);
                FKRoomInviteData invite(clientId);
                foreach(QString user,activeUsers)invite.addUser(user);
                FKBasicEvent joinEvent(FKBasicEventSubject::joinRoom,invite.toVariant());
                server->sendBasicEvent(&joinEvent);
            }
        }
    }
    if(!error.isEmpty()){
        FKConnectionManager* client=_clientConnections.value(clientId,0);
        client->sendMessage(error);
        FKBasicEvent fail(FKBasicEventSubject::joinRoom,false);
        client->sendBasicEvent(&fail);
    }
}

void FKRealm::addClientToRoom(const qint32 server, const QString& client,const QString& room){
    database()->writeValue(FKDBValue(room),_dbPath.clientRoomIndex(client),false);
    database()->addIndex(_dbPath.serverRoomClientIndex(server,client));
}

void FKRealm::removeClientFromRoom(const qint32 server, const QString& client){
    database()->writeValue(FKDBValue(),_dbPath.clientRoomIndex(client),false);
    database()->removeIndex(_dbPath.serverRoomClientIndex(server,client));
    setClientRoomState(client,FKClientRoomState::NotInRoom);
}

void FKRealm::sanityDatabase(){
    //reset room types' avaliable active servers
    FKDBIndex index(_dbPath.roomTypesIndex());
    foreach(QString prop,database()->getProperties(index)){
        database()->cutNode(index>>prop);
        index<<1;
    }
    //reset servers' current rooms
    foreach(auto serverKey,database()->getProperties(_dbPath.serversIndex())){
        qint32 serverId=serverKey.toInt();
        if(serverId>0){
            database()->removeIndex(_dbPath.serverRoomIndex(serverId));
            database()->removeIndex(_dbPath.serverRoomStartedIndex(serverId));
            database()->removeIndex(_dbPath.serverRoomClientsIndex(serverId));
        }else{
            FK_MLOG("Invalid server id found on database sanity")
        }
    }
}

/*!
 * \brief Handle new income guest connection
 */

void FKRealm::incomeConnection(FKConnector* connector){
    FKRealmConnectionManager* mgr=new FKRealmConnectionManager(this,connector,this);
    _guesConnections.insert(mgr);
    emit messageRequested(QString(tr("Got new income connection")));
}

bool FKRealm::isServerConnected(const qint32 id)const{
    return _serverConnections.contains(id);
}

bool FKRealm::isClientConnected(const QString& id)const{
    return _clientConnections.contains(id);
}

void FKRealm::incomeServer(const qint32 id, FKConnector* connector){
    FKRealmConnectionManagerS* mgr=new FKRealmConnectionManagerS(id,this,connector,this);
    _serverConnections.insert(id,mgr);
    foreach(auto roomType,database()->getProperties(_dbPath.serverRoomTypesIndex(id))){
        addServerToRoomType(id,roomType);
    }
    FKBasicEvent submit(FKBasicEventSubject::login,true);
    mgr->sendBasicEvent(&submit);
    emit messageRequested(QString(tr("New server added: "))+QString::number(id));
}

void FKRealm::incomeClient(const QString& id, FKConnector* connector){
    FKRealmConnectionManagerC* mgr=new FKRealmConnectionManagerC(id,this,connector,this);
    _clientConnections.insert(id,mgr);
    FKBasicEvent submit(FKBasicEventSubject::login,true);
    mgr->sendBasicEvent(&submit);
    FKBasicEvent users(FKBasicEventSubject::userList,getUserList(id));
    mgr->sendBasicEvent(&users);
    emit messageRequested(QString(tr("New client added: "))+id);
}

QString FKRealm::getServerPassword(const qint32 id) const{
    return database()->getValue(_dbPath.serverIndex(id),false).string();
}

QString FKRealm::getClientPassword(const QString& id) const{
    return database()->getValue(_dbPath.clientIndex(id),false).string();
}

QStringList FKRealm::getUserList(const QString& id) const{
    return database()->getProperties(_dbPath.clientUsersIndex(id));
}

QStringList FKRealm::getActiveUsers(const QString& id) const{
    return database()->findProperties(_dbPath.clientUsersIndex(id),FKDBValue(true));
}

QStringList FKRealm::getRegisteredRoomTypes() const{
    return database()->getProperties(_dbPath.roomTypesIndex());
}

bool FKRealm::isRoomTypeRegistered(const QString& roomType) const{
    return database()->hasIndex(_dbPath.roomTypeIndex(roomType));
}

bool FKRealm::isRoomTypeRegistered(const QString& roomType, const qint32 serverId) const{
    return database()->hasIndex(_dbPath.serverRoomTypeIndex(serverId,roomType));
}

bool FKRealm::isUserExists(const QString& userName) const{
    return database()->hasIndex(_dbPath.userIndex(userName));
}

bool FKRealm::isUserExists(const QString& userName, const QString& clientId) const{
    return database()->hasIndex(_dbPath.clientUserIndex(clientId,userName));
}

bool FKRealm::isRoomExists(const QString& roomName){
    return _activeRooms.contains(roomName);
}

bool FKRealm::isClientExists(const QString& id) const{
    return database()->hasIndex(_dbPath.clientIndex(id));
}
