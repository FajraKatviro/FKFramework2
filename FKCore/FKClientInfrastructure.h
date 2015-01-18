#ifndef FKCLIENTINFRASTRUCTURE_H
#define FKCLIENTINFRASTRUCTURE_H

#include "FKInfrastructure.h"

#include <QMap>
#include <QStringList>

class FKConnector;
class FKConnectionManager;
class FKUserInfrastructure;

class FKClientInfrastructure : public FKInfrastructure{
    Q_OBJECT
public:
    FKClientInfrastructure(QObject* parent=0);
    ~FKClientInfrastructure();
    FKInfrastructureType infrastructureType()const;
    bool isConnectedToRealm();

    void dropInfrastructure();
    void requestLoginRealm(const QString& id, const QString& password);
    void submitLoginRealm(const QVariant& value);

    void requestUserCreation(const QString& name);
    void requestUserDeletion(const QString& name);
    void requestUserAuthorization(const QString& name);
    void requestUserDeauthorization(const QString& name);
    void requestRoomList(/*filters*/);
    void requestCustomServer();
    void requestCreateRoom(const QString& roomName, const QString& roomType);
    //void requestEnterRoom(const qint32 id);
    //void requestExitRoom();

    void refreshUserList(const QVariant& value);
    void respondUserCreation(const QVariant& value);
    void respondUserDeletion(const QVariant& value);
    void respondUserAuthorization(const QVariant& value);
    void respondUserDeauthorization(const QVariant& value);
    void respondRoomList(const QVariant& value);
    void respondCreateRoom(const QVariant& value);
    //void respondEnterRoom(const QVariant& value);
    //void respondExitRoom(const QVariant& value);
    void respondCustomServer(const QVariant& value);

    QStringList userPool()const;
    QStringList activeUsers()const;

    void messageFromRealm(const QString& msg);
public slots:
    void realmConnection(FKConnector* connector);
signals:
    void connectedToRealm();
    void disconnectedFromRealm();
    void loggedIn();
    void userPoolChanged();
    void activeUsersChanged();
    void roomListChanged();
    void customServerRequested(const qint32 serverId,const QString password);
private slots:
    void connectorStatusChanged();
private:
    bool _logged;
    FKConnectionManager* _realmConnection;
    QMap<QString,FKUserInfrastructure*> _users;
    QStringList _userPool;
    QStringList _roomList;
};

#endif // FKCLIENTINFRASTRUCTURE_H
