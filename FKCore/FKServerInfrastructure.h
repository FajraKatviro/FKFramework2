#ifndef FKSERVERINFRASTRUCTURE_H
#define FKSERVERINFRASTRUCTURE_H

#include "FKInfrastructure.h"
#include "FKInstructionObject.h"

#include <QSet>
#include <QVariant>

#include "FKVersionList.h"

class FKConnector;
class FKConnectionManager;
class FKClientInfrastructureReferent;
class FKServerConnectionManager;
class FKEventObject;
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

    void registerRoomTypeRespond(const QVariant& value);
    void roomTypesNotification(const QVariant& value);
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

    void registerRoomTypeRequest(const QString roomType);
    void removeRoomTypeRequest(const QString roomType);

    void setPort(const qint32 clientsPort);
    void setRealmConnectionSettings(const QString ip,const qint32 port);
    QString realmIP()const;
    qint32 realmPort()const;
    qint32 clientPort()const;
    virtual QString serverIP()const;

    QStringList registeredRoomTypes()const;
    bool waitingForAnswer()const;
    bool isLogged()const;
    bool isConnectedToRealm()const;
    qint32 serverId()const;

    void dispatchEvent(FKEventObject* ev);
    void handleRoomInstruction(FKInstructionObject instruction);
signals:
    void connectedToRealm();
    void disconnectedFromRealm();
    void loggedIn();

    void roomInstruction(FKInstructionObject instruction);
private slots:
    void realmConnectorStatusChanged();
private:
    void roomDataChanged(const qint32 propName, const QVariant value);
    bool checkRealm();
    bool hasRoom()const;
    qint32 roomContextId()const;
    void roomStopped();
    void proccessGuestUsers();
    bool checkInviteData(const FKRoomInviteData& data);
    void syncClient(FKClientInfrastructureReferent* client);
    void dropClient(const QString& clientName);
    void notifyRealmClientDropped(const QString& clientName);
    bool _logged=false;
    FKVersionList _roomVersion;
    FKConnectionManager* _realmConnection=nullptr;
    QSet<FKServerConnectionManager*> _guests;
    QMap<QString,FKClientInfrastructureReferent*> _clients; //this for chat message to client
    QMap<QString,qint32> _users; //this for chat message to user
    QMap<qint32,FKClientInfrastructureReferent*> _referents; //this for event dispatch
    qint32 _serverLoginId=0;
    qint32 _realmPort=-1;
    qint32 _clientsPort=-1;
    QString _realmIP;
    QStringList _registeredRoomTypes;
};

#endif
