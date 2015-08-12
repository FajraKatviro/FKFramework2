#ifndef FKSERVERINFRASTRUCTURE_H
#define FKSERVERINFRASTRUCTURE_H

#include "FKInfrastructure.h"

#include <QSet>

#include "FKIDGenerator.h"

class FKConnector;
class FKConnectionManager;
class FKUserInfrastructureSlot;
class FKUserInfrastructureAlias;
class FKServerConnectionManager;
class FKRoom;
class FKRoomModule;
class FKObjectManager;
class FKRoomData;
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

    void removeRoomTypeRequest(const QString& roomType);
    void removeRoomTypeRespond(const QVariant& value);

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
    void roomDataChanged(const qint32 maxActorsDelta, const qint32 actorsDelta, const qint32 maxUsersDelta, const qint32 usersDelta);
    void roomStarted();
    void roomStopped();
private:
    bool checkRealm();
    bool hasRoom()const;
    bool createRoom(const FKRoomData& roomData);
    bool checkInviteData(const FKRoomInviteData& data);
    virtual FKDataBase* createRoomDatabase();
    bool _logged;
    FKConnectionManager* _realmConnection;
    FKObjectManager* _om;
    FKRoomModule* _roomModule;
    FKRoom* _room;
    QSet<FKServerConnectionManager*> _guests;
    QMap<QString,FKUserInfrastructureAlias*> _clients;
    QMap<QString,FKUserInfrastructureSlot*> _users;
    QMap<qint32,FKUserInfrastructureSlot*> _userObjects;
    FKIDGenerator _idgenerator;
    qint32 _id;
};

#endif
