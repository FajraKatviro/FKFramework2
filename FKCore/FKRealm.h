#ifndef FKREALM_H
#define FKREALM_H

#include "FKInfrastructure.h"

#include <QSet>

class FKConnector;
class FKConnectionManager;
class FKDataBase;

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
    void setDataBase(FKDataBase* db);
    void dropRealm();
    FKInfrastructureType infrastructureType()const;

    void createClientRecord(const QString& clientName, const QString& password);

    void createUser(const QString& clientId, const QVariant& userName);
    void deleteUser(const QString& clientId, const QVariant& userName);
    void selectUser(const QString& clientId, const QVariant& userName);
    void deselectUser(const QString& clientId, const QVariant& userName);
    QStringList userList()const;
public slots:
    void incomeConnection(FKConnector* connector);
private:
    bool isServerConnected(const qint32 id)const;
    bool isClientConnected(const QString& id)const;
    bool isClientId(const QString& id)const;
    bool isUserName(const QString& name)const;
    bool isClientExists(const QString& id)const;
    void incomeServer(const qint32 id,FKConnector* connector);
    void incomeClient(const QString& id,FKConnector* connector);
    QString getServerPassword(const qint32 id)const;
    QString getClientPassword(const QString& id)const;
    QStringList getUserList(const QString& id)const;
    bool isUserExists(const QString& userName)const;
    bool isUserExists(const QString& userName,const QString& clientId)const;
    void createUser(const QString& clientId, const QString& userName);
    void deleteUser(const QString& clientId, const QString& userName);
    void selectUser(const QString& clientId, const QString& userName);
    void deselectUser(const QString& clientId, const QString& userName);
    void createNewClientRecord(const QString& clientName, const QString& password);
    QStringList getRoomList(const QVariant& filters)const;
    qint32 getUserStatus(const QString& clientId, const QString& userName);
    QSet<FKConnectionManager*> _guesConnections;
    QHash<qint32,FKConnectionManager*> _serverConnections;
    QHash<QString,FKConnectionManager*> _clientConnections;
    FKDataBase* _db;
    static const QString clientBranch;
    static const QString serverBranch;
    static const QString userBranch;
    static const QString usersNode;
    static const QString roomNameNode;
    static const qint32 userSelected;
    static const qint32 userNotSelected;
};

#endif // FKREALM_H
