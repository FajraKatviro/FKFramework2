#ifndef FKREALM_H
#define FKREALM_H

#include "FKInfrastructure.h"

#include <QSet>

#include "FKRealmDBMap.h"
#include "FKRoomData.h"

class FKConnector;
class FKConnectionManager;

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

    void registerServerRoomType(const qint32 serverId, const QVariant& data);

    void createUser(const QString& clientId, const QVariant& userName);
    void deleteUser(const QString& clientId, const QVariant& userName);
    void selectUser(const QString& clientId, const QVariant& userName);
    void deselectUser(const QString& clientId, const QVariant& userName);
    void customServerRequested(const QString& clientId, const QVariant& data);
    void createRoomRequested(const QString& clientId, const QVariant& data);
    void enterRoomRequested(const QString& clientId, const QVariant& data);
    void createRoomRespond(const qint32 serverId, const QVariant& data);
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
    QStringList getRegisteredRoomTypes()const;
    bool isRoomTypeRegistered(const QString& roomType)const;
    bool isUserExists(const QString& userName)const;
    bool isUserExists(const QString& userName,const QString& clientId)const;
    bool isRoomExists(const QString& roomName);
    void createUser(const QString& clientId, const QString& userName);
    void deleteUser(const QString& clientId, const QString& userName);
    void selectUser(const QString& clientId, const QString& userName);
    void deselectUser(const QString& clientId, const QString& userName);
    void createNewClientRecord(const QString& clientName, const QString& password);
    qint32 createNewServerRecord(const QString& password);
    void registerNewRoomType(const QString& roomType);
    void registerNewRoomType(const qint32 serverId,const QString& roomType);
    QStringList getRoomList(const QVariant& filters)const;
    qint32 getUserStatus(const QString& clientId, const QString& userName);
    bool hasSelectedUser(const QString& clientId);
    qint32 nextServerId()const;
    bool isClientInRoom(const QString& clientId)const;
    qint32 getCustomServer(const QString& clientId)const;
    //qint32 customServerId(const QString& clientId)const;
    QMap<QString,QVariant> customServerPreserve(const QString& clientId);
    static QString generateServerPassword();
    void setServerRoomData(const qint32 serverId, const QString& roomName, const QString& roomType, const QString& clientId);
    void submitServerRoomData(const qint32 serverId);
    void resetServerRoomData(const qint32 serverId);
    QSet<FKConnectionManager*> _guesConnections;
    QHash<qint32,FKConnectionManager*> _serverConnections;
    QHash<QString,FKConnectionManager*> _clientConnections;
    QHash<QString,FKRoomData> _activeRooms;
    static const qint32 userSelected;
    static const qint32 userNotSelected;
    FKRealmDBMap _dbPath;
};

#endif // FKREALM_H
