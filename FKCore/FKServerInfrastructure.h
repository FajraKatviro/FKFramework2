#ifndef FKSERVERINFRASTRUCTURE_H
#define FKSERVERINFRASTRUCTURE_H

#include "FKInfrastructure.h"

#include <QSet>

#include "FKIDGenerator.h"
#include "FKRoomData.h"

class FKConnector;
class FKConnectionManager;
class FKUserInfrastructureSlot;
class FKUserInfrastructureAlias;
class FKServerConnectionManager;
class FKRoom;
class FKRoomModule;
class FKObjectManager;
class FKRoomInviteData;

class FKServerInfrastructure : public FKInfrastructure{
    Q_OBJECT
public:
    FKServerInfrastructure(QObject* parent=0);
    ~FKServerInfrastructure();
    FKInfrastructureType infrastructureType()const;
    bool isConnectedToRealm()const;

    void dropInfrastructure();
    void requestLoginRealm(const qint32 id, const QString& password);
    void submitLoginRealm(const QVariant& value);

    void registerRoomTypeRequest(const QString& roomType);
    void registerRoomTypeRespond(const QVariant& value);

    void createRoomRequested(const QVariant& data);

    void clientInvited(const QVariant& data);
    //void submitLoginUser(const QVariant& data);

    void messageFromRealm(const QString& msg);

    qint32 serverId()const;

    virtual qint32 userPort()const{return 0;}
public slots:
    void realmConnection(FKConnector* connector);
    //void userConnection(FKConnector* connector);
signals:
    void connectedToRealm();
    void disconnectedFromRealm();
    void loggedIn();
private slots:
    void realmConnectorStatusChanged();
    void roomDataChanged(const qint32 maxActors, const qint32 actors, const qint32 maxUsers, const qint32 users);
    void roomStarted();
    void roomStopped();
private:
    bool checkRealm();
    bool hasRoom()const;
    bool createRoom(const FKRoomData& roomData);
    static QString createUserInvitePassword();
    bool checkInviteData(const FKRoomInviteData& data);
    virtual FKDataBase* createRoomDatabase();
    bool _logged;
    FKConnectionManager* _realmConnection;
    FKObjectManager* _om;
    FKRoomModule* _roomModule;
    FKRoom* _room;
    FKRoomData _roomData;
    QSet<FKServerConnectionManager*> _guests;
    QMap<QString,FKUserInfrastructureAlias*> _clients;
    QMap<QString,FKUserInfrastructureSlot*> _users;
    QMap<qint32,FKUserInfrastructureSlot*> _userObjects;
    FKIDGenerator _idgenerator;
    qint32 _id;
};

#endif
