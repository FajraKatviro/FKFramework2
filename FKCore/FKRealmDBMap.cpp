#include "FKRealmDBMap.h"

#include "FKDataBase.h"

/*!
\class FKRealmDBMap
\brief Provides path-map for easy realm database access
*/

FKDBIndex FKRealmDBMap::serversIndex()const{
    return _servers.path();
}

FKDBIndex FKRealmDBMap::lastServerIndex() const{
    return _lastServer.path();
}

FKDBIndex FKRealmDBMap::serverIndex(const qint32 serverId) const{
    bindServer(serverId);
    return _servers.instance.path();
}

FKDBIndex FKRealmDBMap::serverOwnerIndex(const qint32 serverId) const{
    bindServer(serverId);
    return _servers.instance.owner.path();
}

FKDBIndex FKRealmDBMap::serverRoomTypesIndex(const qint32 serverId)const{
    bindServer(serverId);
    return _servers.instance.roomTypes.path();
}

FKDBIndex FKRealmDBMap::serverRoomTypeIndex(const qint32 serverId, const QString& roomType)const{
    bindServer(serverId);
    _servers.instance.roomTypes.instance.bindNode(roomType);
    return _servers.instance.roomTypes.instance.path();
}

FKDBIndex FKRealmDBMap::clientsIndex() const{
    return _clients.path();
}

FKDBIndex FKRealmDBMap::clientIndex(const QString& clientId) const{
    bindClient(clientId);
    return _clients.instance.path();
}

FKDBIndex FKRealmDBMap::clientLastServerIndex(const QString& clientId) const{
    bindClient(clientId);
    return _clients.instance.lastServer.path();
}

FKDBIndex FKRealmDBMap::clientCustomServerIndex(const QString& clientId) const{
    bindClient(clientId);
    return _clients.instance.customServer.path();
}

FKDBIndex FKRealmDBMap::clientUsersIndex(const QString& clientId)const{
    bindClient(clientId);
    return _clients.instance.users.path();
}

FKDBIndex FKRealmDBMap::clientUserIndex(const QString& clientId, const QString& userId) const{
    bindClient(clientId);
    _clients.instance.users.instance.bindNode(userId);
    return _clients.instance.users.instance.path();
}

FKDBIndex FKRealmDBMap::clientLastUsersIndex(const QString& clientId) const{
    bindClient(clientId);
    return _clients.instance.lastUsers.path();
}

FKDBIndex FKRealmDBMap::clientLastUserIndex(const QString& clientId, const QString& userId) const{
    bindClient(clientId);
    _clients.instance.lastUsers.instance.bindNode(userId);
    return _clients.instance.lastUsers.instance.path();
}

FKDBIndex FKRealmDBMap::usersIndex()const{
    return _users.path();
}

FKDBIndex FKRealmDBMap::userIndex(const QString& userId)const{
    bindUser(userId);
    return _users.instance.path();
}

FKDBIndex FKRealmDBMap::roomTypesIndex()const{
    return _roomTypes.path();
}

FKDBIndex FKRealmDBMap::roomTypeIndex(const QString& roomType) const{
    bindRoomType(roomType);
    return _roomTypes.roomType.path();
}

FKDBIndex FKRealmDBMap::roomTypeServerIndex(const QString& roomType, const qint32 serverId)const{
    bindRoomType(roomType);
    _roomTypes.roomType.server.bindNode(QString::number(serverId));
    return _roomTypes.roomType.server.path();
}

void FKRealmDBMap::bindServer(const qint32 serverId) const{
    _servers.instance.bindNode(QString::number(serverId));
}

void FKRealmDBMap::bindClient(const QString& clientId)const{
    _clients.instance.bindNode(clientId);
}

void FKRealmDBMap::bindUser(const QString& userId)const{
    _users.instance.bindNode(userId);
}

void FKRealmDBMap::bindRoomType(const QString& roomType) const{
    _roomTypes.roomType.bindNode(roomType);
}
