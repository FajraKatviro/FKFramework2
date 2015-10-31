#ifndef FKSIMPLECORE_H
#define FKSIMPLECORE_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include <QVariant>

#include "FKThreadedComponent.h"

#include "fkcore_global.h"

class FKConnector;

class FKCORESHARED_EXPORT FKRealmComponent:public FKThreadedComponent{
    Q_OBJECT
public:
    FKRealmComponent(QObject* parent = nullptr);
    ~FKRealmComponent();
    virtual void startComponent()override;
public slots:
    QStringList userList();
    QStringList userList(const QString clientId);
    QStringList clientList();
    QStringList connectedClientList();
    QStringList activeClientList();
    QStringList serverList();
    QStringList serverList(const QString roomType);
    QList<qint32> connectedServerList();
    QList<int> connectedServerList(const QString roomType);
    QList<qint32> avaliableServerList();
    QList<qint32> avaliableServerList(const QString roomType);
    QStringList roomTypeList();
    QStringList roomTypeList(const qint32 serverId);
    QStringList activeRoomList();
    QStringList activeRoomList(const QVariant filter);

    void createClientRecord(const QString clientName, const QString password);
    void deleteClientRecord(const QString clientName);
    void createServerRecord(const QString password);
    void deleteServerRecord(const qint32 serverId);
    void dropServer(const qint32 serverId);
    void registerRoomType(const QString roomType);
    void removeRoomType(const QString roomType);
//    void createRoomRequest(const QString roomName, const QString roomType);
    void setPort(const qint32 port);
    void guestConnection(FKConnector* connector);
protected slots:
    virtual void componentThreadQuit()override;
signals:
    void messageRequested(const QString msg);
    void started();
};

class FKCORESHARED_EXPORT FKServerComponent:public FKThreadedComponent{
    Q_OBJECT
    Q_PROPERTY(bool waitingForRealmAnswer READ waitingForRealmAnswer NOTIFY waitingForRealmAnswerChanged)
public:
    FKServerComponent(QObject* parent = nullptr);
    ~FKServerComponent();
    virtual void startComponent()override;
public slots:
    QStringList roomTypeList();

    bool isLogged();
    bool isConnectedToRealm();

    void ausvise(const qint32 id, const QString password);
    void registerRoomType(const QString roomType);
    void removeRoomType(const QString roomType);

    void setPort(const qint32 port);
    void setRealmConnectionSettings(const QString realmIP,const qint32 realmPort);
    void realmConnection(FKConnector* connector);
    void guestConnection(FKConnector* connector);
protected slots:
    virtual void componentThreadQuit()override;
signals:
    void messageRequested(const QString msg);
    void started();
    void connectedToRealm();
    void disconnectedFromRealm();
    void waitingForRealmAnswerChanged();
    void loggedIn();
private:
    bool waitingForRealmAnswer();
};

class FKCORESHARED_EXPORT FKClientComponent:public FKThreadedComponent{
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

class FKCORESHARED_EXPORT FKSimpleCore:public QObject{
    Q_OBJECT
    Q_PROPERTY(FKRealmComponent* realm READ realmComponent NOTIFY realmComponentChanged)
    Q_PROPERTY(FKServerComponent* server READ serverComponent NOTIFY serverComponentChanged)
    Q_PROPERTY(FKClientComponent* client READ clientComponent NOTIFY clientComponentChanged)
public:
    FKSimpleCore(QObject* parent=0);
    ~FKSimpleCore();
    inline FKRealmComponent*  realmComponent()const{return _realmComponent;}
    inline FKServerComponent* serverComponent()const{return _serverComponent;}
    inline FKClientComponent* clientComponent()const{return _clientComponent;}
public slots:
    virtual bool startRealmInfrastructure(const qint32 port=0);
    virtual bool startServerInfrastructure(const qint32 port=0, const qint32 realmPort=0,const QString& realmIP=QString());
    //virtual bool startClientInfrastructure(const int realmPort=0,const QString& realmIP=QString())=0;
    virtual bool stopRealmInfrastructure();
    virtual bool stopServerInfrastructure();
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
private slots:
    void realmMessage(QString msg);
    void serverMessage(QString msg);
    void clientMessage(QString msg);
};

#endif
