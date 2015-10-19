#ifndef FKSERVERINFRASTRUCTURE_H
#define FKSERVERINFRASTRUCTURE_H

#include "FKInfrastructure.h"

#include <QSet>
#include <QVariant>

#include "FKIDGenerator.h"

class FKConnector;
class FKConnectionManager;
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

    void dropInfrastructure();
    void submitLoginRealm(const QVariant& value);

    void registerRoomTypeRequest(const QString& roomType);
    void registerRoomTypeRespond(const QVariant& value);

    void removeRoomTypeRequest(const QString& roomType);
    void removeRoomTypeRespond(const QVariant& value);

    void createRoomRequested(const QVariant& data);

    void clientInvited(const QVariant& data);
    void syncRequest(FKServerConnectionManager* guest,const QVariant& data);
    void stopGuestConnection(FKServerConnectionManager* guest);

    void messageFromRealm(const QString& msg);
public slots:
    void realmConnection(FKConnector* connector);
    void clientConnection(FKConnector* connector);
    void requestLoginRealm(const qint32 id, const QString password);

    void setPort(const qint32 clientsPort);
    void setRealmConnectionSettings(const QString ip,const qint32 port);
    QString realmIP()const;
    qint32 realmPort()const;
    qint32 clientPort()const;
    virtual QString serverIP()const;

    bool waitingForAnswer()const;
    bool isLogged()const;
    bool isConnectedToRealm()const;
    qint32 serverId()const;
signals:
    void connectedToRealm();
    void disconnectedFromRealm();
    void loggedIn();
private slots:
    void realmConnectorStatusChanged();
    void roomDataChanged(const qint32 propName, const QVariant value);
    //void clientInviteResolved(const FKRoomInviteData data,const QList<qint32> userObjects);
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
    qint32 _realmPort=-1;
    qint32 _clientsPort=-1;
    QString _realmIP;
};

#endif
