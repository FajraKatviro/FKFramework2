#include "FKRealmDBMap.h"

FKDBIndex FKRealmDBMap::serversIndex(){
    return _servers.path();
}

FKDBIndex FKRealmDBMap::serverIndex(const qint32 serverId){
    bindServer(serverId);
    return _servers.instance.path();
}

FKDBIndex FKRealmDBMap::serverRoomIndex(const qint32 serverId){
    bindServer(serverId);
    return _servers.instance.room.path();
}

FKDBIndex FKRealmDBMap::serverRoomStartedIndex(const qint32 serverId){
    bindServer(serverId);
    return _servers.instance.roomStarted.path();
}

FKDBIndex FKRealmDBMap::serverOwnerIndex(const qint32 serverId){
    bindServer(serverId);
    return _servers.instance.owner.path();
}

FKDBIndex FKRealmDBMap::serverRoomTypesIndex(const qint32 serverId){
    bindServer(serverId);
    return _servers.instance.roomTypes.path();
}

FKDBIndex FKRealmDBMap::serverRoomTypeIndex(const qint32 serverId, const QString& roomType){
    bindServer(serverId);
    _servers.instance.roomTypes.bindNode(roomType);
    return _servers.instance.roomTypes.instance.path();
}

FKDBIndex FKRealmDBMap::clientsIndex(){
    return _clients.path();
}

FKDBIndex FKRealmDBMap::clientIndex(const QString& clientId){
    bindClient(clientId);
    return _clients.instance.path();
}

FKDBIndex FKRealmDBMap::clientRoomIndex(const QString& clientId){
    bindClient(clientId);
    return _clients.instance.currentRoom.path();
}

FKDBIndex FKRealmDBMap::clientCustomServerIndex(const QString& clientId){
    bindClient(clientId);
    return _clients.instance.customServer.path();
}

FKDBIndex FKRealmDBMap::clientUsersIndex(const QString& clientId){
    bindClient(clientId);
    return _clients.instance.users.path();
}

FKDBIndex FKRealmDBMap::clientUserIndex(const QString& clientId, const QString& userId){
    bindClient(clientId);
    _clients.instance.users.instance.bindNode(userId);
    return _clients.instance.users.instance.path();
}

FKDBIndex FKRealmDBMap::usersIndex(){
    return _users.path();
}

FKDBIndex FKRealmDBMap::userIndex(const QString& userId){
    bindUser(userId);
    return _users.instance.path();
}

FKDBIndex FKRealmDBMap::roomTypesIndex(){
    return _rooms.path();
}

FKDBIndex FKRealmDBMap::roomTypeIndex(const QString& roomType){
    bindRoomType(roomType);
    return _rooms.roomType.path();
}

FKDBIndex FKRealmDBMap::roomTypeServerIndex(const QString& roomType, const qint32 serverId){
    bindRoomType(roomType);
    _rooms.roomType.server.bindNode(QString::number(serverId));
    return _rooms.roomType.server.path();
}

void FKRealmDBMap::bindServer(const qint32 serverId){
    _servers.instance.bindNode(QString::number(serverId));
}

void FKRealmDBMap::bindClient(const QString& clientId){
    _clients.instance.bindNode(clientId);
}

void FKRealmDBMap::bindUser(const QString& userId){
    _users.instance.bindNode(userId);
}

void FKRealmDBMap::bindRoomType(const QString& roomType){
    _rooms.roomType.bindNode(roomType);
}
