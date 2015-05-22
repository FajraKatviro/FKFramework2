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

enum class FKClientRoomState{
    NotInRoom,
    CreateRoomRequested,
    EnterRoomRequested,
    Invited
};


class FKRealm : public FKInfrastructure{
    Q_OBJECT
public:
    FKRealm(QObject* parent=0);
    ~FKRealm();
    void ausvise(FKConnectionManager* source,const QVariant loginData);
    void stopGuestConnection(FKConnectionManager* guest);
    void stopServerConnection(const qint32 serverId);
    void stopClientConnection(const QString& clientId);
    void refreshRoomList(const QString& clientId,const QVariant& filters);
    void refreshRoomData(const qint32 serverId, const QVariant& data);
    void dropRealm();
    FKInfrastructureType infrastructureType()const;

    void createClientRecord(const QString& clientName, const QString& password);
    qint32 createServerRecord(const QString& password);
    void registerRoomType(const QString& roomType);
    void createRoomRealmRequest(const QString& roomName,const QString& roomType);

    void registerServerRoomType(const qint32 serverId, const QVariant& data);

    void createUser(const QString& clientId, const QVariant& userName);
    void deleteUser(const QString& clientId, const QVariant& userName);
    void selectUser(const QString& clientId, const QVariant& userName);
    void deselectUser(const QString& clientId, const QVariant& userName);
    void customServerRequested(const QString& clientId, const QVariant& data);
    void createRoomRequested(const QString& clientId, const QVariant& data);
    void enterRoomRequested(const QString& clientId, const QVariant& data);
    void createRoomRespond(const qint32 serverId, const QVariant& data);
    void enterRoomRespond(const qint32 serverId, const QVariant& data);

    void roomStarted(const qint32 serverId, const QVariant& data);
    void roomStopped(const qint32 serverId, const QVariant& data);
    void roomStopped(const qint32 serverId);

    QStringList userList()const;
public slots:
    void incomeConnection(FKConnector* connector);
private:
    bool isServerConnected(const qint32 id)const;
    bool isClientConnected(const QString& id)const;
    bool isClientExists(const QString& id)const;
    void incomeServer(const qint32 id,FKConnector* connector);
    void incomeClient(const QString& id,FKConnector* connector);
    QString getServerPassword(const qint32 id)const;
    QString getClientPassword(const QString& id)const;
    QStringList getUserList(const QString& id)const;
    QStringList getActiveUsers(const QString& id)const;
    QStringList getRegisteredRoomTypes()const;
    bool isRoomTypeRegistered(const QString& roomType)const;
    bool isRoomTypeRegistered(const QString& roomType,const qint32 serverId)const;
    bool isUserExists(const QString& userName)const;
    bool isUserExists(const QString& userName,const QString& clientId)const;
    bool isRoomExists(const QString& roomName);
    void createUser(const QString& clientId, const QString& userName);
    void deleteUser(const QString& clientId, const QString& userName);
    void selectUser(const QString& clientId, const QString& userName);
    void deselectUser(const QString& clientId, const QString& userName);
    void createNewClientRecord(const QString& clientName, const QString& password);
    qint32 createNewServerRecord(const QString& password);
    qint32 getFreeServer(const QString& roomType)const;
    void registerNewRoomType(const QString& roomType);
    void registerNewRoomType(const qint32 serverId,const QString& roomType);
    QStringList getRoomList(const QVariant& filters)const;
    QString getServerRoom(const qint32 serverId)const;
    QString getClientRoom(const QString& clientId)const;
    qint32 getUserActive(const QString& clientId, const QString& userName);
    bool hasSelectedUser(const QString& clientId)const;
    qint32 nextServerId()const;
    FKClientRoomState getClientRoomState(const QString& clientId)const;
    void setClientRoomState(const QString& clientId, FKClientRoomState state)const;
    bool isCustomServerRequested(const QString& clientId)const;
    qint32 getCustomServer(const QString& clientId)const;
    QString getServerIP(const qint32 serverId);
    FKAusviceData customServerPreserve(const QString& clientId);
    bool processInviteData(const FKRoomInviteData& invite);
    QString createNewRoomForServer(const QString& roomName,const QString& roomType,const QString& owner=QString());
    static QString generateServerPassword();
    const FKRoomData& setServerRoomData(const qint32 serverId, const QString& roomName, const QString& roomType, const QString& clientId, const bool custom);
    void submitRoomData(const QString& roomId);
    void abortRoomData(const qint32 serverId,const QString& roomId);
    void enterRoomRequested(const QString& clientId, const QString& roomId);
    void addClientToRoom(const qint32 server, const QString& client, const QString& room);
    void removeClientFromRoom(const qint32 server, const QString& client);
    void addServerToRoomType(const qint32 serverId,const QString& roomType);

    void sanityDatabase()override;

    QSet<QString> _customServerRequestedClients;
    QSet<FKConnectionManager*> _guesConnections;
    QHash<qint32,FKConnectionManager*> _serverConnections;
    QHash<QString,FKConnectionManager*> _clientConnections;
    QHash<QString,FKRoomData> _activeRooms;
    const FKRealmDBMap _dbPath;
};

#endif // FKREALM_H
