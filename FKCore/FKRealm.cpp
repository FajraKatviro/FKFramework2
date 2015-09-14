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
Realm has own database for information storing.
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
    auto server=_serverConnections.value(serverId,nullptr);
    if(server){
        roomStopped(serverId);
        _serverConnections.remove(serverId);
        server->mgr->dropConnection();
        server->mgr->deleteLater();
        delete server;
        emit messageRequested(QString(tr("Server %1 connection stopped")).arg(QString(serverId)));
    }else{
        FK_MLOGV("Failed stop server connection (not found)",serverId)
    }
}

/*!
 * \brief Drop client connection, remove manager and delete it
 */

void FKRealm::stopClientConnection(const QString& clientId){
    FKConnectionManager* client=_clientConnections.value(clientId,nullptr);
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
    sanityDatabase();
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

void FKRealm::deleteClientRecord(const QString& clientName){
    if(clientName.isEmpty()){
        emit messageRequested(QString(tr("Unable delete client: empty name provided")));
        return;
    }
    if(!isClientId(clientName)){
        emit messageRequested(QString(tr("Unable delete client: invalid name provided")));
        return;
    }
    if(!isClientExists(clientName)){
        emit messageRequested(QString(tr("Unable delete client: not exists")));
        return;
    }
    if(isClientConnected(clientName)){
        emit messageRequested(QString(tr("Unable delete client record for connected client")));
        return;
    }
    if(isClientOnServer(clientName)){
        emit messageRequested(QString(tr("Unable delete client record for client connected to server")));
        return;
    }
    if(isServerConnected(getCustomServer(clientName))){
        emit messageRequested(QString(tr("Unable delete client if it's custom server started")));
        return;
    }

    deleteClientRecordFromDatabase(clientName);
    emit messageRequested(QString(tr("Client record deletion success")));
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

void FKRealm::deleteServerRecord(const qint32 serverId){
    if(serverId<=0){
        emit messageRequested(QString(tr("Unable delete server record: invalid id provided")));
        return;
    }
    if(isServerConnected(serverId)){
        emit messageRequested(QString(tr("Unable delete connected server")));
        return;
    }
    if(!isServerRegistered(serverId)){
        emit messageRequested(QString(tr("Unable delete server: record does not exists")));
        return;
    }
    if(!getServerOwner(serverId).isEmpty()){
        emit messageRequested(QString(tr("Unable delete custom server, try delete client record instead")));
        return;
    }

    deleteServerRecordFromDatabase(serverId);
    emit messageRequested(QString(tr("Server record deletion success")));
}

/*!
 * \brief Register room type to allow request rooms of that type.
 */

void FKRealm::registerRoomType(const QString& roomType){
    QString error;
    bool check=true;
    if(roomType.isEmpty()){
        error=QString(tr("Unable register room type: empty type"));
        check=false;
    }else if(!isRoomTypeName(roomType)){
        error=QString(tr("Unable register room type: invalid type"));
        check=false;
    }else if(isRoomTypeRegistered(roomType)){
        error=QString(tr("Unable register room type: %1 already registered")).arg(roomType);
        check=false;
    }
    if(check){
        registerNewRoomType(roomType);
        emit messageRequested(QString(tr("Room type %1 registered")).arg(roomType));
    }else{
        emit messageRequested(error);
    }
}

void FKRealm::removeRoomType(const QString& roomType){
    QString error;
    bool check=true;
    if(roomType.isEmpty()){
        error=QString(tr("Unable remove room type: empty type"));
        check=false;
    }else if(!isRoomTypeName(roomType)){
        error=QString(tr("Unable remove room type: invalid type"));
        check=false;
    }else if(!isRoomTypeRegistered(roomType)){
        error=QString(tr("Unable remove room type: %1 not registered")).arg(roomType);
        check=false;
    }else if(hasActiveServersForRoomType(roomType)){
        error=QString(tr("Unable remove room type while there are active servers has %1 registered")).arg(roomType);
        check=false;
    }
    if(check){
        removeRoomTypeFromDatabase(roomType);
        emit messageRequested(QString(tr("Room type %1 removed from realm")).arg(roomType));
    }else{
        emit messageRequested(error);
    }
}

void FKRealm::createRoomRealmRequest(const QString& roomName, const QString& roomType){
    QString error;
    bool check=true;
    if(roomType.isEmpty()){
        error=QString(tr("Unable create room: empty type"));
        check=false;
    }else if(!isRoomTypeName(roomType)){
        error=QString(tr("Unable create room: invalid type"));
        check=false;
    }else if(!isRoomTypeRegistered(roomType)){
        error=QString(tr("Unable create room: type %1 not registered")).arg(roomType);
        check=false;
    }else if(roomName.isEmpty()){
        error=QString(tr("Unable create room: empty name"));
        check=false;
    }else if(!isRoomName(roomName)){
        error=QString(tr("Unable create room: invalid name"));
        check=false;
    }else if(isRoomExists(roomName)){
        error=QString(tr("Unable create room: name already in use"));
        check=false;
    }

    if(check){
        qint32 serverId=getFreeServer(roomType);
        if(serverId>0){
            createNewRoomForServer(roomName,roomType,serverId);
        }else{
            error=QString(tr("Unable create room: no avaliable servers"));
            check=false;
        }
    }
    if(!check)emit messageRequested(error);
}

void FKRealm::registerServerRoomType(const qint32 serverId, const QVariant& data){
    QString error, roomType(data.toString());
    bool check=true;
    if(roomType.isEmpty()){
        error=QString(tr("Unable register room type: empty type"));
        check=false;
    }else if(!isRoomTypeName(roomType)){
        error=QString(tr("Unable register room type: invalid type"));
        check=false;
    }else if(!isRoomTypeRegistered(roomType)){
        error=QString(tr("Unable register room type: room type not registered at realm")).arg(roomType);
        check=false;
    }else if(isRoomTypeRegistered(roomType,serverId)){
        error=QString(tr("Unable register room type: %1 already registered")).arg(roomType);
        check=false;
    }
    FKConnectionManager* connector=_serverConnections.value(serverId)->mgr;
    if(check){
        registerNewRoomType(serverId,roomType);
    }else{
        connector->sendMessage(error);
    }
    FKBasicEvent answer(FKBasicEventSubject::registerRoomType,check);
    connector->sendBasicEvent(&answer);
}

void FKRealm::removeServerRoomType(const qint32 serverId, const QVariant& data){
    QString error, roomType(data.toString());
    bool check=true;
    if(roomType.isEmpty()){
        error=QString(tr("Unable remove room type: empty type"));
        check=false;
    }else if(!isRoomTypeName(roomType)){
        error=QString(tr("Unable remove room type: invalid type"));
        check=false;
    }else if(!isRoomTypeRegistered(roomType,serverId)){
        error=QString(tr("Unable remove room type: room type not registered for server"));
        check=false;
    }else if(getServerRoomType(serverId)==roomType){
        error=QString(tr("Unable remove room type while room of that type is started"));
        check=false;
    }
    FKConnectionManager* connector=_serverConnections.value(serverId)->mgr;
    if(check){
        removeRoomTypeFromDatabase(serverId,roomType);
    }else{
        connector->sendMessage(error);
    }
    FKBasicEvent answer(FKBasicEventSubject::removeRoomType,check);
    connector->sendBasicEvent(&answer);
}

/*!
 * \brief Try create user for given client and send reply
 */

void FKRealm::createUser(const QString& clientId, const QVariant& userName){
    QString errorMessage;
    bool check=true;
    QString name=userName.toString();
    if(name.isEmpty()){
        errorMessage=QString(tr("Unable create user: invalid argument"));
        check=false;
    }else if(!isUserName(name)){
        errorMessage=QString(tr("Unable create user: invalid username"));
        check=false;
    }else if(isUserExists(name)){
        errorMessage=QString(tr("Unable create user: username already in use"));
        check=false;
    }

    FKConnectionManager* mgr=_clientConnections.value(clientId);
    if(check){
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
    bool check=true;
    QString name=userName.toString();
    if(name.isEmpty()){
        errorMessage=QString(tr("Unable delete user: invalid argument"));
        check=false;
    }else if(!isUserName(name)){
        errorMessage=QString(tr("Unable delete user: invalid username"));
        check=false;
    }else if(!isUserExists(name,clientId)){
        errorMessage=QString(tr("Unable delete user: no such user found"));
        check=false;
    }else if(isClientInRoom(clientId)){
        errorMessage=QString(tr("Unable delete user while client in room"));
        check=false;
    }

    FKConnectionManager* mgr=_clientConnections.value(clientId);
    if(check){
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
    FKRoomRequestData roomData(data);
    QString error, roomName(roomData.roomId()), roomType(roomData.roomType());
    QStringList users(roomData.users());
    bool check=true;
    if(roomName.isEmpty()){
        error=QString(tr("Room request declined: empty room name provided"));
        check=false;
    }else if(roomType.isEmpty()){
        error=QString(tr("Room request declined: empty room type provided"));
        check=false;
    }else if(!isRoomName(roomName)){
        error=QString(tr("Room request declined: invalid room name provided"));
        check=false;
    }else if(!isRoomTypeName(roomType)){
        error=QString(tr("Room request declined: invalid room type provided"));
        check=false;
    }else if(!isRoomTypeRegistered(roomType)){
        error=QString(tr("Room request declined: room type not registered"));
        check=false;
    }else if(isRoomExists(roomName)){
        error=QString(tr("Room request declined: room name already in use"));
        check=false;
    }else if(users.isEmpty()){
        error=QString(tr("Room request declined: no users provided"));
        check=false;
    }else if(users.removeDuplicates()){
        error=QString(tr("Room request declined: duplicate users provided"));
        check=false;
    }else{
        foreach(QString user,users){
            if(user.isEmpty()){
                error=QString(tr("Room request declined: empty user name provided"));
                check=false;
                break;
            }else if(!isUserName(user)){
                error=QString(tr("Room request declined: invalid user name provided"));
                check=false;
                break;
            }else if(!isUserExists(user,clientId)){
                error=QString(tr("Room request declined: user %1 not registered for %2 client")).arg(user).arg(clientId);
                check=false;
                break;
            }
        }
    }

    if(check){
        todo;//check if custom
        qint32 serverId=getFreeServer(roomType);
        if(serverId>0){
            createNewRoomForServer(roomName,roomType,serverId,clientId);
            addClientToRoom(serverId,clientId,users);
        }else{
            error=QString(tr("Unable create room: no avaliable servers"));
            check=false;
        }
    }
    if(!check){
       FKConnectionManager* clientMgr=_clientConnections.value(clientId,nullptr);
       clientMgr->sendMessage(error);
       FKBasicEvent ev(FKBasicEventSubject::createRoom);
       clientMgr->sendBasicEvent(&ev);
    }
}

void FKRealm::createNewRoomForServer(const QString& roomName, const QString& roomType, const qint32 serverId, const QString& creator){
    bool custom=!getServerOwner(serverId).isEmpty();
    auto s=_serverConnections.value(serverId,nullptr);
    s->room=roomName;
    QVariant creationRequest=_activeRooms.insert(roomName,FKRoomData(serverId,roomType,creator,QDateTime::currentDateTime(),custom)).value().toCreationRequest();
    emit messageRequested(QString(tr("Room %1 of type %2 creation request sent to %3 server")).arg(roomName).arg(roomType).arg(QString::number(serverId)));
    FKBasicEvent ev(FKBasicEventSubject::createRoom,creationRequest);
    s->mgr->sendBasicEvent(&ev);
}

void FKRealm::joinRoomRequested(const QString& clientId, const QVariant& data){
    FKRoomRequestData roomData(data);
    QString error, roomName(roomData.roomId());
    QStringList users(roomData.users());
    bool check=true;
    if(roomName.isEmpty()){
        error=QString(tr("Room join request declined: empty room name provided"));
        check=false;
    }else if(!isRoomName(roomName)){
        error=QString(tr("Room join request declined: invalid room name provided"));
        check=false;
    }else if(!isRoomExists(roomName)){
        error=QString(tr("Room join request declined: room does not exists"));
        check=false;
    }else if(isClientInRoom(clientId)){
        error=QString(tr("Room join request declined: already joined room"));
        check=false;
    }else if(users.isEmpty()){
        error=QString(tr("Room join request declined: no users provided"));
        check=false;
    }else if(users.removeDuplicates()){
        error=QString(tr("Room join request declined: duplicate users provided"));
        check=false;
    }else{
        foreach(QString user,users){
            if(user.isEmpty()){
                error=QString(tr("Room join request declined: empty user name provided"));
                check=false;
                break;
            }else if(!isUserName(user)){
                error=QString(tr("Room join request declined: invalid user name provided"));
                check=false;
                break;
            }else if(!isUserExists(user,clientId)){
                error=QString(tr("Room join request declined: user %1 not registered for %2 client")).arg(user).arg(clientId);
                check=false;
                break;
            }
        }
    }

    if(check){
       addClientToRoom(getRoomServer(roomId),clientId,users);
    }else{
       FKConnectionManager* clientMgr=_clientConnections.value(clientId,nullptr);
       clientMgr->sendMessage(error);
       FKBasicEvent ev(FKBasicEventSubject::joinRoom);
       clientMgr->sendBasicEvent(&ev);
    }
}

//void FKRealm::createRoomRespond(const qint32 serverId, const QVariant& data){
//    QString room=getServerRoom(serverId);
//    if(room.isEmpty()){
//        emit messageRequested(QString(tr("Unexpected room creation respond from %1 server")).arg(QString::number(serverId)));
//        return;
//    }
//    bool success=data==QVariant(true);
//    QString client=_activeRooms[room].owner();
//    if(!client.isEmpty()){
//        if(getClientRoomState(client)!=FKClientRoomState::CreateRoomRequested){
//            emit messageRequested(QString(tr("Invalid client room state when room created on %1 server")).arg(QString::number(serverId)));
//        }else{
//            setClientRoomState(client,FKClientRoomState::NotInRoom);
//        }
//    }
//    FKConnectionManager* mgr=_clientConnections.value(client,0);

//    if(success){
//        submitRoomData(room);
//    }else{
//        abortRoomData(serverId,room);
//    }

//    if(!client.isEmpty()){
//        if(mgr){
//            FKBasicEvent creation(FKBasicEventSubject::createRoom,success);
//            mgr->sendBasicEvent(&creation);
//            if(success){
//                enterRoomRequested(client,room);
//            }
//        }else{
//            emit messageRequested(QString(tr("Unable respond creating room client %1: connection not found")).arg(client));
//        }
//    }
//}

//void FKRealm::enterRoomRespond(const qint32 serverId, const QVariant& data){
//    QString room=getServerRoom(serverId);
//    FKRoomInviteData invite(data);
//    QString error;
//    if(room.isEmpty()){
//        error=QString(tr("Unexpected room enter respond from %1 server")).arg(QString::number(serverId));
//    }else if(invite.client().isEmpty()){
//        error=QString(tr("Invalid room enter respond from %1 server: no client provided")).arg(QString::number(serverId));
//    }else if(!isClientId(invite.client())){
//        error=QString(tr("Invalid room enter respond from %1 server: invalid client id")).arg(QString::number(serverId));
//    }else if(!isClientExists(invite.client())){
//        error=QString(tr("Invalid room enter respond from %1 server: no client provided")).arg(QString::number(serverId));
//    }else if(getClientRoomState(invite.client())!=FKClientRoomState::EnterRoomRequested){
//        error=QString(tr("Invalid room enter respond from %1 server: wrong client state (%2)")).arg(QString::number(serverId)).arg(QString::number((qint32)getClientRoomState(invite.client())));
//    }else if(getClientRoom(invite.client())!=room){
//        error=QString(tr("Invalid room enter respond from %1 server: client room miss-match")).arg(QString::number(serverId));
//    }

//    if(!error.isEmpty()){
//        emit messageRequested(error);
//        return;
//    }

//    QVariant ret;
//    bool success=false;
//    FKConnectionManager* mgr=_clientConnections.value(invite.client(),0);
//    if(mgr){
//        if(invite.isValid()){
//            if(processInviteData(invite)){
//                setClientRoomState(invite.client(),FKClientRoomState::Invited);
//                invite.setAddress(getServerIP(serverId));
//                ret=invite.toVariant();
//                success=true;
//            }
//        }
//    }
//    if(!success){
//        removeClientFromRoom(serverId,invite.client());
//        FKRoomInviteData fail;
//        ret=fail.toVariant();
//    }
//    if(mgr){
//        FKBasicEvent answer(FKBasicEventSubject::joinRoom,ret);
//        mgr->sendBasicEvent(&answer);
//    }else{
//        emit messageRequested(QString(tr("Unable reply enter room to client %1: connection not found")).arg(invite.client()));
//    }
//}

//void FKRealm::roomStarted(const qint32 serverId, const QVariant& data){
//    Q_UNUSED(data)
//    QString room=getServerRoom(serverId);
//    auto i=_activeRooms.find(room);
//    if(i!=_activeRooms.end()){
//        database()->writeValue(FKDBValue(true),_dbPath.serverRoomStartedIndex(serverId),false);
//        _activeRooms.erase(i);
//    }else{
//        emit messageRequested(QString(tr("Unexpected room started event from %1 server: no room found")).arg(QString::number(serverId)));
//    }
//}

void FKRealm::roomStopped(const qint32 serverId, const QVariant& data){
    Q_UNUSED(data)
    roomStopped(serverId);
}

void FKRealm::roomStopped(const qint32 serverId){
    auto s=_serverConnections.value(serverId,nullptr);
    QString room(s->room);
    if(!room.isEmpty()){
        auto i=_activeRooms.find(room);
        if(i!=_activeRooms.end()){
            _activeRooms.erase(i);
        }
        foreach(auto client,s->clients){
            database()->removeIndex(_dbPath.clientLastServerIndex(client));
            database()->removeIndex(_dbPath.clientLastUsersIndex(client));
        }
        s->room.clear();
    }
}

/*!
 * \brief Return list of users registered in database
 */

QStringList FKRealm::userList() const{
    return database()->getProperties(_dbPath.usersIndex());
}

void FKRealm::createUser(const QString& clientId, const QString& userName){
    database()->addIndex(_dbPath.clientUserIndex(clientId,userName));
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
    qint32 serverId=takeNextServerId();
    database()->writeValue(FKDBValue(password),_dbPath.clientIndex(clientName),false);
    database()->writeValue(FKDBValue(serverId),_dbPath.clientCustomServerIndex(clientName),false);
    database()->writeValue(FKDBValue(password),_dbPath.serverIndex(serverId),false);
    database()->writeValue(FKDBValue(clientName),_dbPath.serverOwnerIndex(clientName),false);
}

void FKRealm::deleteClientRecordFromDatabase(const QString& clientName){
    foreach(QString user,getUserList(clientName)){
        database()->removeIndex(_dbPath.userIndex(user));
    }
    deleteServerRecordFromDatabase(getCustomServer(clientName));
    database()->removeIndex(_dbPath.clientIndex(clientName));
}

qint32 FKRealm::createNewServerRecord(const QString& password){
    qint32 id=takeNextServerId();
    database()->writeValue(FKDBValue(password),_dbPath.serverIndex(id),false);
    return id;
}

void FKRealm::deleteServerRecordFromDatabase(const qint32 serverId){
    foreach(QString roomType,database()->getProperties(_dbPath.serverRoomTypesIndex(serverId))){
        database()->removeIndex(_dbPath.roomTypeServerIndex(roomType,serverId));
    }
    database()->removeIndex(_dbPath.serverIndex(serverId));
}

qint32 FKRealm::getFreeServer(const QString& roomType) const{
    for(auto s=_serverConnections.constBegin();s!=_serverConnections.constEnd();++s){
        FKServerReferent* ref=s.value();
        if(ref->room.isEmpty() && ref->roomTypes.contains(roomType) && ref->owner.isEmpty())
            return s.key();
    }
    return -1;
}

qint32 FKRealm::getRoomServer(const QString& roomId) const{
    auto d=_activeRooms.constFind(roomId);
    return d==_activeRooms.constEnd() ? -1 : d.value().server();
}

void FKRealm::registerNewRoomType(const QString& roomType){
    database()->addIndex(_dbPath.roomTypeIndex(roomType));
}

void FKRealm::removeRoomTypeFromDatabase(const QString& roomType){
    foreach(QString server,getServersForRoomType(roomType)){
        database()->removeIndex(_dbPath.serverRoomTypeIndex(server.toInt(),roomType));
    }
    database()->removeIndex(_dbPath.roomTypeIndex(roomType));
}

void FKRealm::registerNewRoomType(const qint32 serverId, const QString& roomType){
    database()->addIndex(_dbPath.serverRoomTypeIndex(serverId,roomType));
    database()->addIndex(_dbPath.roomTypeServerIndex(roomType,serverId));
    auto server=_serverConnections.value(serverId,nullptr);
    if(server)server->roomTypes.append(roomType);
}

void FKRealm::removeRoomTypeFromDatabase(const qint32 serverId, const QString& roomType){
    database()->removeIndex(_dbPath.serverRoomTypeIndex(serverId.toInt(),roomType));
    database()->removeIndex(_dbPath.roomTypeServerIndex(roomType,serverId));
    auto server=_serverConnections.value(serverId,nullptr);
    if(server)server->roomTypes.removeAll(roomType);
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

QString FKRealm::getClientRoom(const QString& clientId) const{
    return database()->getValue(_dbPath.clientRoomIndex(clientId),false).string();
}

//qint32 FKRealm::getUserActive(const QString& clientId, const QString& userName){
//    return database()->getValue(_dbPath.clientUserIndex(clientId,userName)).boolean();
//}

//bool FKRealm::hasSelectedUser(const QString& clientId) const{
//    return database()->findIndex(FKDBValue(true),_dbPath.clientUsersIndex(clientId)).isValid();
//}

qint32 FKRealm::takeNextServerId() const{
    qint32 id=database()->getValue(_dbPath.lastServerIndex(),false).number()+1;
    database()->writeValue(FKDBValue(id),_dbPath.lastServerIndex(),false);
    return id;
}

//FKClientRoomState FKRealm::getClientRoomState(const QString& clientId) const{
//    return FKClientRoomState(database()->getValue(_dbPath.clientRoomStateIndex(clientId),false).number());
//}

//void FKRealm::setClientRoomState(const QString& clientId, FKClientRoomState state) const{
//    database()->writeValue(FKDBValue(qreal(state)),_dbPath.clientRoomStateIndex(clientId),false);
//}

bool FKRealm::isCustomServerRequested(const QString& clientId) const{
    return _customServerRequestedClients.contains(clientId);
}

qint32 FKRealm::getCustomServer(const QString& clientId) const{
    if(clientId.isEmpty())return 0;
    return database()->getValue(_dbPath.clientCustomServerIndex(clientId),false).number();
}

QStringList FKRealm::getServersForRoomType(const QString& roomType) const{
    return database()->getProperties(_dbPath.roomTypeIndex(roomType));
}

QString FKRealm::getServerRoomType(const qint32 serverId) const{
    auto server=_serverConnections.value(serverId,nullptr);
    if(server){
        if(!server->room.isEmpty()){
            auto r=_activeRooms.find(server->room);
            if(r!=_activeRooms.end()){
                return r.value().roomType();
            }
        }
    }
    return QString();
}

QString FKRealm::getServerIP(const qint32 serverId){
    QString ip;
    auto server=_serverConnections.value(serverId,0);
    if(server){
        ip=server->mgr->address();
    }
    return ip;
}

//FKAusviceData FKRealm::customServerPreserve(const QString& clientId){
//    qint32 serverId;
//    QString serverPassword;

//    FKDBIndex ind=_dbPath.clientCustomServerIndex(clientId);
//    if(database()->hasIndex(ind)){
//        serverId=database()->getValue(ind,false).number();
//        serverPassword=database()->getValue(_dbPath.serverIndex(serverId),false).string();
//    }else{
//        serverPassword=generateServerPassword();
//        serverId=createServerRecord(serverPassword);
//        database()->writeValue(FKDBValue(qreal(serverId)),ind,false);
//    }
//    _customServerRequestedClients.insert(clientId);
//    database()->writeValue(FKDBValue(clientId),_dbPath.serverOwnerIndex(serverId),false);
//    return FKAusviceData(serverId,serverPassword);
//}

//bool FKRealm::processInviteData(const FKRoomInviteData& invite){
//    QStringList activeUsers(getActiveUsers(invite.client()));
//    QMap<QString,QString> inviteUsers(invite.passwords());
//    if(inviteUsers.size()!=activeUsers.size()){
//        emit messageRequested(QString(tr("Invalid invite request from server: user list size does not match")));
//        return false;
//    }
//    for(auto i=inviteUsers.constBegin();i!=inviteUsers.constEnd();++i){
//        if(activeUsers.removeAll(i.key())!=1){
//            emit messageRequested(QString(tr("Invalid invite request from server: user list does not match")));
//            return false;
//        }
//        if(i.value().isEmpty()){
//            emit messageRequested(QString(tr("Invalid invite request from server: empty password provided")));
//            return false;
//        }
//    }
//    return true;
//}

QString FKRealm::generateServerPassword(){
    return createRandomString(10,20);
}

//void FKRealm::submitRoomData(const QString& roomId){
//    _activeRooms[roomId].setReady(true);
//    emit messageRequested(QString(tr("Room creation submitted: %1")).arg(roomId));
//}

//void FKRealm::abortRoomData(const qint32 serverId, const QString& roomId){
//    _activeRooms.remove(roomId);
//    database()->setValue(FKDBValue(),_dbPath.serverRoomIndex(serverId),false);
//    emit messageRequested(QString(tr("Room creation aborted: %1")).arg(roomId));
//}

//void FKRealm::enterRoomRequested(const QString& clientId, const QString& roomId){
//    auto i=_activeRooms.constFind(roomId);
//    QString error;
//    if(i==_activeRooms.constEnd()){
//        error=QString(tr("Unable join room %1: room not found")).arg(roomId);
//    }else{
//        const FKRoomData& roomData=i.value();
//        QStringList activeUsers(getActiveUsers(clientId));
//        qint32 newUsersCount=activeUsers.count();
//        if(roomData.users()+newUsersCount>roomData.maximumUsers()){
//            error=QString(tr("Unable join room %1: not enough user slots")).arg(roomId);
//        }else{
//            auto server=_serverConnections.value(roomData.server(),0);
//            if(!server){
//                error=QString(tr("Unable join room %1: server not found")).arg(roomId);
//            }else{
//                addClientToRoom(roomData.server(),clientId,roomId); //this call to remove client from room on room stopped later
//                setClientRoomState(clientId,FKClientRoomState::EnterRoomRequested);
//                FKRoomInviteData invite(clientId);
//                foreach(QString user,activeUsers)invite.addUser(user);
//                FKBasicEvent joinEvent(FKBasicEventSubject::joinRoom,invite.toVariant());
//                server->mgr->sendBasicEvent(&joinEvent);
//            }
//        }
//    }
//    if(!error.isEmpty()){
//        FKConnectionManager* client=_clientConnections.value(clientId,0);
//        client->sendMessage(error);
//        FKBasicEvent fail(FKBasicEventSubject::joinRoom,false);
//        client->sendBasicEvent(&fail);
//    }
//}

QString FKRealm::createUserInvitePassword()const{
    return createRandomString(8,10);
}

void FKRealm::addClientToRoom(const qint32 server, const QString& client, const QStringList& users){
    FKServerReferent* s=_serverConnections.value(server,nullptr);
    FKRoomInviteData invite(client,createUserInvitePassword());
    invite.setPort(s->mgr->guestPort());
    invite.setAddress(s->mgr->address());
    foreach(QString user,users){
        if(!invite.addUser(user)){
            FK_MLOG("Happend something very bad")
        }
        database()->addIndex(_dbPath.clientLastUserIndex(client,user));
    }
    database()->writeValue(FKDBValue(server),_dbPath.clientLastServerIndex(client),false);
    s->clients.append(client);

    FKBasicEvent serverInvite(FKBasicEventSubject::joinRoom,invite.toServerInvite());
    s->mgr->sendBasicEvent(&serverInvite);

    FKBasicEvent clientInvite(FKBasicEventSubject::joinRoom,invite.toClientInvite());
    FKConnectionManager* clientMgr=_clientConnections.value(client,nullptr);
    clientMgr->sendBasicEvent(&clientInvite);
}

//void FKRealm::removeClientFromRoom(const qint32 server, const QString& client){
//    database()->writeValue(FKDBValue(),_dbPath.clientRoomIndex(client),false);
//    database()->removeIndex(_dbPath.serverRoomClientIndex(server,client));
//    setClientRoomState(client,FKClientRoomState::NotInRoom);
//}

void FKRealm::sanityDatabase(){
//    //reset room types' avaliable active servers
//    FKDBIndex index(_dbPath.roomTypesIndex());
//    foreach(QString prop,database()->getProperties(index)){
//        database()->cutNode(index>>prop);
//        index<<1;
//    }
//    //reset servers' current rooms
//    foreach(auto serverKey,database()->getProperties(_dbPath.serversIndex())){
//        qint32 serverId=serverKey.toInt();
//        if(serverId>0){
//            database()->removeIndex(_dbPath.serverRoomIndex(serverId));
//            database()->removeIndex(_dbPath.serverRoomStartedIndex(serverId));
//            database()->removeIndex(_dbPath.serverRoomClientsIndex(serverId));
//        }else{
//            FK_MLOG("Invalid server id found on database sanity")
//        }
//    }
    foreach(QString client,database()->getProperties(_dbPath.clientsIndex())){
        database()->removeIndex(_dbPath.clientLastServerIndex(client));
        database()->removeIndex(_dbPath.clientLastUsersIndex(client));
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

bool FKRealm::isServerRegistered(const qint32 id) const{
    return database()->hasIndex(_dbPath.serverIndex(id));
}

bool FKRealm::isClientConnected(const QString& id)const{
    return _clientConnections.contains(id);
}

bool FKRealm::isClientOnServer(const QString& id) const{
    return database()->hasIndex(_dbPath.clientLastServerIndex(id));
}

void FKRealm::incomeServer(const qint32 id, FKConnector* connector){
    FKServerReferent* server=new FKServerReferent();
    server->mgr=new FKRealmConnectionManagerS(id,this,connector,this);
    server->roomTypes=database()->getProperties(_dbPath.serverRoomTypesIndex(id));
    server->owner=getServerOwner(id);
    _serverConnections.insert(id,server);
    FKBasicEvent submit(FKBasicEventSubject::login,true);
    mgr->sendBasicEvent(&submit);
    emit messageRequested(QString(tr("New server added: "))+QString::number(id));
}

void FKRealm::incomeClient(const QString& id, FKConnector* connector){
    FKRealmConnectionManagerC* mgr=new FKRealmConnectionManagerC(id,this,connector,this);
    _clientConnections.insert(id,mgr);
    FKBasicEvent submit(FKBasicEventSubject::login,true);
    mgr->sendBasicEvent(&submit);
    emit messageRequested(QString(tr("New client added: "))+id);
    QStringList users(getUserList(id));
    if(!users.isEmpty()){
        FKBasicEvent refreshUsers(FKBasicEventSubject::userList,users);
        mgr->sendBasicEvent(&refreshUsers);
    }
}

QString FKRealm::getServerPassword(const qint32 id) const{
    return database()->getValue(_dbPath.serverIndex(id),false).string();
}

QString FKRealm::getClientPassword(const QString& id) const{
    return database()->getValue(_dbPath.clientIndex(id),false).string();
}

QString FKRealm::getServerOwner(const qint32 id) const{
    return database()->getValue(_dbPath.serverOwnerIndex(id),false).string();
}

QStringList FKRealm::getUserList(const QString& clientId) const{
    return database()->getProperties(_dbPath.clientUsersIndex(clientId));
}

bool FKRealm::isClientInRoom(const QString& clientId) const{
    return !getLastRoom(clientId).isEmpty();
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

bool FKRealm::hasActiveServersForRoomType(const QString& roomType) const{
    foreach(QString s,getServersForRoomType(roomType)){
        if(isServerConnected(s.toInt()))
            return true;
    }
    return false;
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
