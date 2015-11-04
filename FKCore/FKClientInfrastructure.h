#ifndef FKCLIENTINFRASTRUCTURE_H
#define FKCLIENTINFRASTRUCTURE_H

#include "FKInfrastructure.h"

#include <QMap>
#include <QStringList>
#include <QSharedPointer>

#include "FKVersionList.h"

class FKConnector;
class FKConnectionManager;
class FKUpdateChannel;

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

    void submitLoginServer(const QVariant& value);
    void syncComplete(const QVariant& value);

    bool requestUserCreation(const QString& name);
    bool requestUserDeletion(const QString& name);
    void requestRoomList(/*filters*/);
    void requestCreateRoom(const QString& roomName, const QString& roomType, const QStringList& users, const bool custom);
    void requestJoinRoom(const QString& roomName, const QStringList& users);
    //void requestEnterRoom(const qint32 id);
    //void requestExitRoom();

    void refreshUserList(const QVariant& value);
    void respondUserCreation(const QVariant& value);
    void respondUserDeletion(const QVariant& value);
    void respondRoomList(const QVariant& value);
    void respondEnterRoom(const QVariant& value);
    void respondCreateRoom(const QVariant& value);
    //void respondExitRoom(const QVariant& value);
    //void respondCustomServer(const QVariant& value);

    void incomeVersionData(const QVariant& value);

    QStringList userPool()const;
    //QStringList activeUsers()const;
    QList<QSharedPointer<FKUpdateChannel>> updates()const;

    void messageFromRealm(const QString& msg);

    void setCustomServerId(const qint32 serverId);

//    qint32 realmPort()const;
//    QString realmIP()const;
    QString roomVisualizer()const;
public slots:
    bool waitingForRealmAnswer()const;
    bool waitingForServerAnswer()const;

    void setRealmConnectionSettings(const QString ip,const qint32 port);
    QString realmIP()const;
    qint32 realmPort()const;

    void realmConnection(FKConnector* connector);
    void serverConnection(FKConnector* connector);
signals:
    void connectedToRealm();
    void disconnectedFromRealm();
    void connectedToServer();
    void disconnectedFromServer();
    void loggedIn();
    void loggedInServer();
    void userPoolChanged();
    void activeUsersChanged();
    void roomListChanged();
    void connectToServerRequest(const QString address,const qint32 port);
    //void updateListChanged();
    //void customServerRequested(const qint32 serverId,const QString password);

    void roomModuleChanged();

    void waitingForRealmAnswerChanged();
    void waitingForServerAnswerChanged();
private slots:
    void connectorStatusChanged();
    void serverConnectorStatusChanged();
    void splitWaitingAnswer(FKInfrastructureType t);
private:
    void requestLoginServer();
    void startUser(const qint32 objectId);
    bool _logged=false;
    bool _serverLogged=false;
    QString _clientId;
    QString _dynamicPassword;
    FKConnectionManager* _realmConnection=nullptr;
    FKConnectionManager* _serverConnection=nullptr;
    QStringList _userPool;
    QStringList _roomList;
    qint32 _customServerId=0;

    qint32 _realmPort=-1;
    QString _realmIP;

    struct{
        QList<QSharedPointer<FKUpdateChannel>> channels;
        //qint32 size=0;
    }_updates;
};

#endif // FKCLIENTINFRASTRUCTURE_H
