#ifndef FKREALMDBMAP_H
#define FKREALMDBMAP_H

#include "FKDataBasePath.h"

struct FKRealmDBServerRoomTypes:public FKDataBasePathCommon{
    FKRealmDBServerRoomTypes(const QString& path,FKDataBasePath* parent)
        :FKDataBasePathCommon(path,parent)
        ,instance(this){}
    FKDataBasePath instance;
};

struct FKRealmDBServerRoomClients:public FKDataBasePathCommon{
    FKRealmDBServerRoomClients(const QString& path,FKDataBasePath* parent)
        :FKDataBasePathCommon(path,parent)
        ,instance(this){}
    FKDataBasePath instance;
};

struct FKRealmDBServer:public FKDataBasePath{
    FKRealmDBServer(FKDataBasePath* parent)
        :FKDataBasePath(parent)
        ,room("room",this)
    //    ,roomStarted("started",this)
        ,owner("owner",this)
        ,roomTypes("roomTypes",this){}
    //    ,roomClients("roomClients",this){}
    FKDataBasePathCommon room;
    //FKDataBasePathCommon roomStarted;
    FKDataBasePathCommon owner;
    FKRealmDBServerRoomTypes roomTypes;
    //FKRealmDBServerRoomClients roomClients;
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
        ,lastServer("server",this)
        ,customServer("custom",this)
        ,users("users",this)
        ,lastUsers("last",this){}
    FKDataBasePathCommon lastServer;
    FKDataBasePathCommon customServer;
    FKRealmDBClientUsers users;
    FKRealmDBClientUsers lastUsers;
};

struct FKRealmDBClients:public FKDataBasePathCommon{
    FKRealmDBClients(const QString& path,FKDataBasePath* parent)
        :FKDataBasePathCommon(path,parent)
        ,instance(this){}
    FKRealmDBClient instance;
};

struct FKRealmDBUsers:public FKDataBasePathCommon{
    FKRealmDBUsers(const QString& path,FKDataBasePath* parent)
        :FKDataBasePathCommon(path,parent)
        ,instance(this){}
    FKDataBasePath instance;
};

struct FKRealmDBRoomTypeServers:public FKDataBasePath{
    FKRealmDBRoomTypeServers(FKDataBasePath* parent)
        :FKDataBasePath(parent)
        ,server(this){}
    FKDataBasePath server;
};

struct FKRealmDBRoomTypes:public FKDataBasePathCommon{
    FKRealmDBRoomTypes(const QString& path,FKDataBasePath* parent)
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
      ,_roomTypes("roomTypes",this)
      ,_lastServer("lastServer",this){}

    FKDBIndex serversIndex()const;
    FKDBIndex lastServerIndex()const;
    FKDBIndex serverIndex(const qint32 serverId)const;
    FKDBIndex serverOwnerIndex(const qint32 serverId)const;
    FKDBIndex serverRoomTypesIndex(const qint32 serverId)const;
    FKDBIndex serverRoomTypeIndex(const qint32 serverId,const QString& roomType)const;

    FKDBIndex clientsIndex()const;
    FKDBIndex clientIndex(const QString& clientId)const;
    FKDBIndex clientLastServerIndex(const QString& clientId)const;
    FKDBIndex clientCustomServerIndex(const QString& clientId)const;
    FKDBIndex clientUsersIndex(const QString& clientId)const;
    FKDBIndex clientUserIndex(const QString& clientId,const QString& userId)const;
    FKDBIndex clientLastUsersIndex(const QString& clientId)const;
    FKDBIndex clientLastUserIndex(const QString& clientId,const QString& userId)const;

    FKDBIndex usersIndex()const;
    FKDBIndex userIndex(const QString& userId)const;

    FKDBIndex roomTypesIndex()const;
    FKDBIndex roomTypeIndex(const QString& roomType)const;
    FKDBIndex roomTypeServerIndex(const QString& roomType,const qint32 serverId)const;
private:
    void bindServer(const qint32 serverId)const;
    void bindClient(const QString& clientId)const;
    void bindUser(const QString& userId)const;
    void bindRoomType(const QString& roomType)const;
    const FKRealmDBServers _servers;
    const FKRealmDBClients _clients;
    const FKRealmDBUsers _users;
    const FKRealmDBRoomTypes _roomTypes;
    const FKDataBasePathCommon _lastServer;
};

#endif // FKREALMDBMAP_H
