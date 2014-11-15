#ifndef FKSERVERINFRASTRUCTURE_H
#define FKSERVERINFRASTRUCTURE_H

#include "FKInfrastructure.h"

class FKConnector;
class FKConnectionManager;

class FKServerInfrastructure : public FKInfrastructure{
    Q_OBJECT
public:
    FKServerInfrastructure(QObject* parent=0);
    ~FKServerInfrastructure();
    FKInfrastructureType infrastructureType()const;
    bool isConnectedToRealm();

    void dropInfrastructure();
    void requestLoginRealm(const qint32 id, const QString& password);
    void submitLoginRealm(const QVariant& value);

    void requestUserCreation(const QString& name);
    void requestUserDeletion(const QString& name);
    void requestUserAuthorization(const QString& name);
    void requestUserDeauthorization(const QString& name);
    void requestRoomList(/*filters*/);
    //void requestCreateRoom(/*params*/);
    //void requestEnterRoom(const qint32 id);
    //void requestExitRoom();

    void refreshUserList(const QVariant& value);
    void respondUserCreation(const QVariant& value);
    void respondUserDeletion(const QVariant& value);
    void respondUserAuthorization(const QVariant& value);
    void respondUserDeauthorization(const QVariant& value);
    void respondRoomList(const QVariant& value);
    //void respondCreateRoom(const QVariant& value);
    //void respondEnterRoom(const QVariant& value);
    //void respondExitRoom(const QVariant& value);

    QStringList userPool()const;
    QStringList activeUsers()const;

    void messageFromRealm(const QString& msg);
public slots:
    void realmConnection(FKConnector* connector);
    void userConnection(FKConnector* connector);
signals:
    void connectedToRealm();
    void disconnectedFromRealm();
    void loggedIn();
    void userPoolChanged();
    void activeUsersChanged();
    void roomListChanged();
private slots:
    void connectorStatusChanged();
private:
    bool _logged;
    FKConnectionManager* _realmConnection;
    QMap<QString,FKUserInfrastructure*> _users;
    QStringList _userPool;
    QStringList _roomList;
};

#endif
