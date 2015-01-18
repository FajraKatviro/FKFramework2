#ifndef FKREALMDBMAP_H
#define FKREALMDBMAP_H

#include "FKDataBasePath.h"

struct FKRealmDBServerRoomTypes:public FKDataBasePathCommon{
    FKRealmDBServerRoomTypes(const QString& path,FKDataBasePath* parent)
        :FKDataBasePathCommon(path,parent)
        ,instance(this){}
    FKDataBasePath instance;
};

struct FKRealmDBServer:public FKDataBasePath{
    FKRealmDBServer(FKDataBasePath* parent)
        :FKDataBasePath(parent)
        ,room("room",this)
        ,owner("owner",this)
        ,roomTypes("roomTypes",this){}
    FKDataBasePathCommon room;
    FKDataBasePathCommon owner;
    FKRealmDBServerRoomTypes roomTypes;
};

struct FKRealmDBServers:public FKDataBasePathCommon{
    FKRealmDBServers(const QString& path,FKDataBasePath* parent)
        :FKDataBasePathCommon(path,parent)
        ,instance(this){}
    FKRealmDBServer instance;
};

struct FKRealmDBClientUsers:public FKDataBasePathCommon{
    FKRealmDBClientUsers(const QString& path,FKDataBasePath* parent)
        :FKDataBasePathCommon(path,parent)
        ,instance(this){}
    FKDataBasePath instance;
};

struct FKRealmDBClient:public FKDataBasePath{
    FKRealmDBClient(FKDataBasePath* parent)
        :FKDataBasePath(parent)
        ,currentRoom("room",this)
        ,customServer("custom",this)
        ,users("users",this){}
    FKDataBasePathCommon currentRoom;
    FKDataBasePathCommon customServer;
    FKRealmDBClientUsers users;
};

struct FKRealmDBClients:public FKDataBasePathCommon{
    FKRealmDBClients(const QString& path,FKDataBasePath* parent)
        :FKDataBasePathCommon(path,parent)
        ,instance(this){}
    FKRealmDBClient instance;
};

struct FKRealmDBUsers:public FKDataBasePathCommon{
    FKRealmDBUsers(const QString& path,FKDataBasePath* parent)
        :FKDataBasePathCommon(path,this)
        ,instance(this){}
    FKDataBasePath instance;
};

struct FKRealmDBRoomTypeServers:public FKDataBasePath{
    FKRealmDBRoomTypeServers(FKDataBasePath* parent)
        :FKDataBasePath(parent)
        ,server(this){}
    FKDataBasePath server;
};

struct FKRealmDBRooms:public FKDataBasePathCommon{
    FKRealmDBRooms(const QString& path,FKDataBasePath* parent)
        :FKDataBasePathCommon(path,parent)
        ,roomType(this){}
    FKRealmDBRoomTypeServers roomType;
};

class FKRealmDBMap:public FKDataBasePath{
public:
    FKRealmDBMap()
        :FKDataBasePath()
      ,_servers("servers",this)
      ,_clients("clients",this)
      ,_users("users",this)
      ,_rooms("rooms",this){}

    FKDBIndex serversIndex();
    FKDBIndex serverIndex(const qint32 serverId);
    FKDBIndex serverRoomIndex(const qint32 serverId);
    FKDBIndex serverOwnerIndex(const qint32 serverId);
    FKDBIndex serverRoomTypesIndex(const qint32 serverId);
    FKDBIndex serverRoomTypeIndex(const qint32 serverId,const QString& roomType);

    FKDBIndex clientsIndex();
    FKDBIndex clientIndex(const QString& clientId);
    FKDBIndex clientRoomIndex(const QString& clientId);
    FKDBIndex clientCustomServerIndex(const QString& clientId);
    FKDBIndex clientUsersIndex(const QString& clientId);
    FKDBIndex clientUserIndex(const QString& clientId,const QString& userId);

    FKDBIndex usersIndex();
    FKDBIndex userIndex(const QString& userId);

    FKDBIndex roomTypesIndex();
    FKDBIndex roomTypeIndex(const QString& roomType);
    FKDBIndex roomTypeServerIndex(const QString& roomType,const qint32 serverId);
private:
    void bindServer(const qint32 serverId);
    void bindClient(const QString& clientId);
    void bindUser(const QString& userId);
    void bindRoomType(const QString& roomType);
    FKRealmDBServers _servers;
    FKRealmDBClients _clients;
    FKRealmDBUsers _users;
    FKRealmDBRooms _rooms;
};

#endif // FKREALMDBMAP_H
