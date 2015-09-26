#ifndef FKSERVERINFRASTRUCTURE_H
#define FKSERVERINFRASTRUCTURE_H

#include "FKInfrastructure.h"

#include <QSet>

#include "FKIDGenerator.h"

class FKConnector;
class FKConnectionManager;
class FKUserInfrastructureSlot;
class FKClientInfrastructureReferent;
class FKServerConnectionManager;
class FKEventObject;
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
    void syncRequest(FKServerConnectionManager* guest,const QVariant& data);

    void messageFromRealm(const QString& msg);

    qint32 serverId()const;

    virtual qint32 userPort()const{return 0;}
public slots:
    void realmConnection(FKConnector* connector);
    void clientConnection(FKConnector* connector);
signals:
    void connectedToRealm();
    void disconnectedFromRealm();
    void loggedIn();
private slots:
    void realmConnectorStatusChanged();
    void roomDataChanged(const QString propName, const QVariant value);
    void clientInviteResolved(const FKRoomInviteData data,const QList<qint32> userObjects);
    //void roomStarted();
    void roomStopped();
    void dispatchEvent(FKEventObject* ev);
private:
    bool checkRealm();
    bool hasRoom()const;
    bool createRoom(const FKRoomData& roomData);
    bool checkInviteData(const FKRoomInviteData& data);
    void syncClient(FKClientInfrastructureReferent* client);
    void dropClient(const QString& clientName);
    void notifyRealmClientDropped(const QString& clientName);
    virtual FKDataBase* createRoomDatabase();
    bool _logged;
    FKConnectionManager* _realmConnection;
    FKObjectManager* _om;
    FKRoomModule* _roomModule;
    FKRoom* _room;
    QSet<FKServerConnectionManager*> _guests;
    QMap<QString,FKClientInfrastructureReferent*> _clients; //this for chat message to client
    QMap<QString,qint32> _users; //this for chat message to user
    QMap<qint32,FKClientInfrastructureReferent*> _referents; //this for event dispatch
    FKIDGenerator _idgenerator;
    qint32 _id;
};

#endif
