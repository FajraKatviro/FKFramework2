#ifndef FKABSTRACTCORE_H
#define FKABSTRACTCORE_H

#include <QObject>
#include <QMap>
#include <QStringList>

#include "fkcore_global.h"

class QDir;

class FKRealm;
class FKServerInfrastructure;
class FKClientInfrastructure;
class FKUserInfrastructure;
enum class FKInfrastructureType:qint8;

class FKCORESHARED_EXPORT FKAbstractCore:public QObject{
    Q_OBJECT
    Q_PROPERTY(bool waitingForRealmAnswer READ waitingForRealmAnswer NOTIFY waitingForRealmAnswerChanged)
    Q_PROPERTY(bool waitingForServerAnswer READ waitingForServerAnswer NOTIFY waitingForServerAnswerChanged)
    Q_PROPERTY(QString uiSource READ uiSource NOTIFY uiSourceChanged)
public:
    FKAbstractCore(QObject* parent=0);
    ~FKAbstractCore();
signals:
    void messageRequested(const QString msg);
    void systemMessageRequested(const QString msg);
    void clientConnectedToRealm();
    void clientDisconnectedFromRealm();
    void clientLoggedIn();
    void clientConnectedToServer();
    void clientDisconnectedFromServer();
    void clientLoggedInServer();
    void clientGotUpdateList();
    void serverConnectedToRealm();
    void serverDisconnectedFromRealm();
    void serverLoggedIn();
    void waitingForRealmAnswerChanged();
    void waitingForServerAnswerChanged();
    void realmStarted();
    void uiSourceChanged();
public slots:
    virtual bool startRealm(const int port=0)=0;
    virtual bool startServer(const int port=0, const int realmPort=0,const QString& realmIP=QString())=0;
    virtual bool startClientInfrastructure(const int realmPort=0,const QString& realmIP=QString())=0;
    void ausviseClientInfrastructure(const QString id, const QString password);
    void ausviseServerInfrastructure(const qint32 id, const QString password);
    bool createUserRecord(const QString& name);
    bool deleteUserRecord(const QString& name);

    virtual bool stopRealm()=0;
    virtual bool stopServer()=0;
    virtual bool stopClientManager()=0;

    //void setApplicationFilesList(){_appFilesList=buildApplicationFilesList();}
    //const FKFilesList& applicationFilesList()const{return _appFilesList;}

    void quitApplication();
    //void restartApplication();

    bool waitingForRealmAnswer()const;
    bool waitingForServerAnswer()const;
    QStringList userList()const;
    QStringList realmUserList()const;

    void createClientRecord(const QString clientName, const QString password);
    void deleteClientRecord(const QString clientName);
    void createServerRecord(const QString password);
    void deleteServerRecord(const qint32 serverId);
    void registerRoomType(const QString roomType);
    void registerServerRoomType(const QString roomType);
    void removeRoomType(const QString roomType);
    void removeServerRoomType(const QString roomType);

    void createRoomRequest(const QString roomName, const QString roomType, const QStringList users);
    void joinRoomRequest(const QString roomName, const QStringList users);
    void createRoomRequestRealm(const QString roomName, const QString roomType);
    //void createCustomServerRequest();
    QString uiSource()const;
protected:
    //void addPostExecutionCommand(const QString& command);
    //virtual FKFilesList buildApplicationFilesList()const;

    void setRealm(FKRealm* realm);
    void setServer(FKServerInfrastructure* server);
    void setClientInfrastructure(FKClientInfrastructure* infr);
    FKRealm* realm();
    FKServerInfrastructure* server();
    FKClientInfrastructure* clientInfrastructure();

    virtual QString realmDatabasePath()const;
    virtual QString serverDatabasePath()const;
    static QDir& changeDir(QDir& dir,const QString& name);
private slots:
    void waitingForAnswerChanged(FKInfrastructureType t);
    void serverConnectedToRealmSlot();
    void serverDisconnectedFromRealmSlot();
    void serverLoggedInSlot();

    virtual void connectClientToServer(const QString address,const qint32 port);
private:
    //FKFilesList _appFilesList;

    FKRealm* _realm;
    FKServerInfrastructure* _server;
    FKClientInfrastructure* _infr;

    //qint32 _customServerId;
    //QString _customServerPassword;
};



#endif // FKABSTRACTCORE_H
