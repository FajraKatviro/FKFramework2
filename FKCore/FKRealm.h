#ifndef FKREALM_H
#define FKREALM_H

#include "FKInfrastructure.h"

#include <QSet>

#include "FKRealmDBMap.h"
#include "FKRoomData.h"

class FKConnector;
class FKConnectionManager;
class FKAusviceData;
class FKRoomInviteData;

class FKRealm : public FKInfrastructure{
    Q_OBJECT
public:
    FKRealm(QObject* parent=0);
    ~FKRealm();
    void ausvise(FKConnectionManager* source,const QVariant loginData);
    void stopGuestConnection(FKConnectionManager* guest);
    void stopServerConnection(const qint32 serverId);
    void stopClientConnection(const QString& clientId);
    //void refreshRoomList(const QString& clientId,const QVariant& filters);
    void refreshRoomData(const qint32 serverId, const QVariant& data);
    void dropRealm();
    FKInfrastructureType infrastructureType()const;

    void createClientRecord(const QString& clientName, const QString& password);
    void deleteClientRecord(const QString& clientName);
    qint32 createServerRecord(const QString& password);
    void deleteServerRecord(const qint32 serverId);
    void registerRoomType(const QString& roomType);
    void removeRoomType(const QString& roomType);
    void createRoomRealmRequest(const QString& roomName,const QString& roomType);

    void registerServerRoomType(const qint32 serverId, const QVariant& data);
    void removeServerRoomType(const qint32 serverId, const QVariant& data);

    void createUser(const QString& clientId, const QVariant& userName);
    void deleteUser(const QString& clientId, const QVariant& userName);
    void createRoomRequested(const QString& clientId, const QVariant& data);
    void joinRoomRequested(const QString& clientId, const QVariant& data);
    //void createRoomRespond(const qint32 serverId, const QVariant& data);
    //void enterRoomRespond(const qint32 serverId, const QVariant& data);

    void clientDisonnectedFromServer(const qint32 serverId,const QVariant& data);

    void roomStopped(const qint32 serverId, const QVariant& data); //implementation for connector
    void roomStopped(const qint32 serverId);
public slots:
    virtual void setPort(const qint32 /*port*/){}
    void incomeConnection(FKConnector* connector);

    QStringList userList()const;
    QStringList getUserList(const QString clientId)const;

    QStringList clientList()const;
    QStringList connectedClientList()const;
    QStringList activeClientList()const;

    QStringList serverList()const;
    QList<qint32> connectedServerList()const;
    QStringList serverAvaliableRoomTypes(const qint32 serverId)const;

    QStringList getRegisteredRoomTypes()const;
    QStringList getServersForRoomType(const QString roomType)const;

    QStringList getRoomList(const QVariant filters)const;
private:
    bool isServerConnected(const qint32 id)const;
    bool isServerRegistered(const qint32 id)const;
    bool isClientConnected(const QString& id)const;
    bool isClientOnServer(const QString& id)const;
    bool isClientExists(const QString& id)const;
    void incomeServer(const qint32 id,FKConnector* connector);
    void incomeClient(const QString& id,FKConnector* connector);
    QString getServerPassword(const qint32 id)const;
    QString getClientPassword(const QString& id)const;
    QString getServerOwner(const qint32 id)const;
    QStringList getLastUsers(const QString& clientId)const;
    QString getLastRoom(const QString& clientId)const;
    bool isClientInRoom(const QString& clientId)const;
    bool isRoomTypeRegistered(const QString& roomType)const;
    bool hasActiveServersForRoomType(const QString& roomType)const;
    bool isRoomTypeRegistered(const QString& roomType,const qint32 serverId)const;
    bool isUserExists(const QString& userName)const;
    bool isUserExists(const QString& userName,const QString& clientId)const;
    bool isRoomExists(const QString& roomName);
    void createUser(const QString& clientId, const QString& userName);
    void deleteUser(const QString& clientId, const QString& userName);
    void createNewClientRecord(const QString& clientName, const QString& password);
    void deleteClientRecordFromDatabase(const QString& clientName);
    qint32 createNewServerRecord(const QString& password);
    void deleteServerRecordFromDatabase(const qint32 serverId);
    qint32 getFreeServer(const QString& roomType)const;
    qint32 getRoomServer(const QString& roomId)const;
    void registerNewRoomType(const QString& roomType);
    void removeRoomTypeFromDatabase(const QString& roomType);
    void registerNewRoomType(const qint32 serverId,const QString& roomType);
    void removeRoomTypeFromDatabase(const qint32 serverId,const QString& roomType);
    //QString getServerRoom(const qint32 serverId)const;
    //QString getClientRoom(const QString& clientId)const;
    //qint32 getUserActive(const QString& clientId, const QString& userName);
    //bool hasSelectedUser(const QString& clientId)const;
    qint32 takeNextServerId()const;
    //FKClientRoomState getClientRoomState(const QString& clientId)const;
    //void setClientRoomState(const QString& clientId, FKClientRoomState state)const;
    //bool isCustomServerRequested(const QString& clientId)const;
    qint32 getCustomServer(const QString& clientId)const;
    QStringList getServersForRoomType(const QString& roomType)const;
    QString getServerRoomType(const qint32 serverId)const;
    //QString getServerIP(const qint32 serverId);
    //FKAusviceData customServerPreserve(const QString& clientId);
    //bool processInviteData(const FKRoomInviteData& invite);
    void createNewRoomForServer(const QString& roomName, const QString& roomType, const qint32 serverId, const QString& creator=QString());
    static QString generateServerPassword();
    //const FKRoomData& setServerRoomData(const qint32 serverId, const QString& roomName, const QString& roomType, const QString& clientId, const bool custom);
    //void submitRoomData(const QString& roomId);
    //void abortRoomData(const qint32 serverId,const QString& roomId);
    //void enterRoomRequested(const QString& clientId, const QString& roomId);
    QString createUserInvitePassword()const;
    void addClientToRoom(const qint32 server, const QString& client, const QStringList& users);
    //void removeClientFromRoom(const qint32 server, const QString& client);
    //void addServerToRoomType(const qint32 serverId,const QString& roomType);

    void sanityDatabase()override;
    void restructureDatabase();

    struct FKServerReferent{
        FKConnectionManager* mgr;
        QStringList clients;
        QStringList roomTypes;
        QString room;
        QString owner;
    };

    QSet<FKConnectionManager*> _guestConnections;
    QHash<qint32,FKServerReferent*> _serverConnections;
    QHash<QString,FKConnectionManager*> _clientConnections;
    QHash<QString,FKRoomData> _activeRooms;
    const FKRealmDBMap _dbPath;
};

#endif // FKREALM_H
