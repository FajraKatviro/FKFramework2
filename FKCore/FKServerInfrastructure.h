#ifndef FKSERVERINFRASTRUCTURE_H
#define FKSERVERINFRASTRUCTURE_H

#include "FKInfrastructure.h"

#include <QSet>

#include "FKIDGenerator.h"

class FKConnector;
class FKConnectionManager;
class FKUserInfrastructureSlot;
class FKClientInfrastructureSlot;
class FKServerConnectionManager;
class FKRoom;
class FKObjectManager;

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

    //void roomCreateRequest(const QVariant& data);
    //void clientInvited(const QVariant& data);
    //void submitLoginUser(const QVariant& data);

    void messageFromRealm(const QString& msg);
public slots:
    void realmConnection(FKConnector* connector);
    //void userConnection(FKConnector* connector);
signals:
    void connectedToRealm();
    void disconnectedFromRealm();
    void loggedIn();
private slots:
    void realmConnectorStatusChanged();
private:
    //bool checkInviteData(const QVariant& data, QString& client, QMap<QString,QString>& userMap);
    //static FKRoom* createRoom(const QString& roomType);
    //static void unloadRoomType(const QString& roomType);
    //static QStringList getRoomTypeList();
    bool _logged;
    FKConnectionManager* _realmConnection;
    FKObjectManager* _om;
    FKRoom* _room;
    QSet<FKServerConnectionManager*> _guests;
    QMap<QString,FKClientInfrastructureSlot*> _invitedClients;
    QMap<QString,FKUserInfrastructureSlot*> _invitedUsers;
    QMap<QString,FKClientInfrastructureSlot*> _clients;
    QMap<QString,FKUserInfrastructureSlot*> _users;
    QMap<qint32,FKUserInfrastructureSlot*> _userObjects;
    FKIDGenerator _idgenerator;
};

#endif
