#ifndef FKABSTRACTCORE_H
#define FKABSTRACTCORE_H

#include <QObject>
#include <QMap>
#include <QStringList>

#include "FKThreadedComponent.h"

#include "fkcore_global.h"

class FKConnector;

class FKRealmComponent:FKThreadedComponent{
    Q_OBJECT
public:
    FKRealmComponent(QObject* parent = nullptr);
    ~FKRealmComponent();
    virtual void startComponent()override;
public slots:
    QStringList userList()const;
    QStringList userList(const QString clientId)const;
    QStringList clientList()const;
    QStringList connectedClientList()const;
    QStringList activeClientList()const;
    QStringList serverList()const;
    QStringList serverList(const QString roomType)const;
    QList<qint32> connectedServerList()const;
    QStringList roomTypeList()const;
    QStringList roomTypeList(const qint32 serverId)const;
    QStringList activeRoomList()const;
    QStringList activeRoomList(const QVariant filter)const;

//    void createClientRecord(const QString clientName, const QString password);
//    void deleteClientRecord(const QString clientName);
//    void createServerRecord(const QString password);
//    void deleteServerRecord(const qint32 serverId);
//    void registerRoomType(const QString roomType);
//    void removeRoomType(const QString roomType);
//    void createRoomRequest(const QString roomName, const QString roomType);
    void setPort(const qint32 port);
    void guestConnection(FKConnector* connector);
signals:
    void messageRequested(const QString msg);
    void started();
};

class FKServerComponent:FKThreadedComponent{
    Q_OBJECT
public:
    FKServerComponent(QObject* parent = nullptr);
    ~FKServerComponent();
    //virtual void startComponent()override;
public slots:
//    void ausvise(const qint32 id, const QString password);
//    void registerRoomType(const QString roomType);
//    void removeRoomType(const QString roomType);
signals:
    void messageRequested(const QString msg);
    void connectedToRealm();
    void disconnectedFromRealm();
    void loggedIn();
};

class FKClientComponent:FKThreadedComponent{
    Q_OBJECT
    //Q_PROPERTY(bool waitingForRealmAnswer READ waitingForRealmAnswer NOTIFY waitingForRealmAnswerChanged)
    //Q_PROPERTY(bool waitingForServerAnswer READ waitingForServerAnswer NOTIFY waitingForServerAnswerChanged)
public:
    FKClientComponent(QObject* parent = nullptr);
    ~FKClientComponent();
//    virtual void startComponent()override;
//    bool waitingForRealmAnswer()const;
//    bool waitingForServerAnswer()const;
public slots:
//    void ausvise(const QString id, const QString password);
//    void createUserRecord(const QString& name);
//    void deleteUserRecord(const QString& name);
//    void createRoomRequest(const QString roomName, const QString roomType, const QStringList users);
//    void joinRoomRequest(const QString roomName, const QStringList users);
//    QStringList userList()const;
signals:
    void messageRequested(const QString msg);
    void waitingForRealmAnswerChanged();
    void waitingForServerAnswerChanged();

    void connectedToRealm();
    void disconnectedFromRealm();
    void loggedInRealm();
    void connectedToServer();
    void disconnectedFromServer();
    void loggedInServer();
    void gotUpdateList();
};

class FKCORESHARED_EXPORT FKAbstractCore:public QObject{
    Q_OBJECT
    Q_PROPERTY(FKRealmComponent* realmComponent READ realmComponent NOTIFY realmComponentChanged)
    Q_PROPERTY(FKServerComponent* serverComponent READ serverComponent NOTIFY serverComponentChanged)
    Q_PROPERTY(FKClientComponent* clientComponent READ clientComponent NOTIFY clientComponentChanged)
public:
    FKAbstractCore(QObject* parent=0);
    ~FKAbstractCore();
    inline FKRealmComponent*  realmComponent()const{return _realmComponent;}
    inline FKServerComponent* serverComponent()const{return _serverComponent;}
    inline FKClientComponent* clientComponent()const{return _clientComponent;}
public slots:
    virtual bool startRealmInfrastructure(const qint32 port=0);
    //virtual bool startServerInfrastructure(const int port=0, const int realmPort=0,const QString& realmIP=QString())=0;
    //virtual bool startClientInfrastructure(const int realmPort=0,const QString& realmIP=QString())=0;
    virtual bool stopRealmInfrastructure();
    //virtual bool stopServerInfrastructure()=0;
    //virtual bool stopClientInfrastructure()=0;
    void quitApplication();
    void initComponents();
signals:
    void messageRequested(const QString msg);
    void realmComponentChanged();
    void serverComponentChanged();
    void clientComponentChanged();
protected:
    virtual void createComponents();
    virtual void installComponents();
    virtual void installComponentFactories();

    FKRealmComponent* _realmComponent=nullptr;
    FKServerComponent* _serverComponent=nullptr;
    FKClientComponent* _clientComponent=nullptr;
};

#endif // FKABSTRACTCORE_H
