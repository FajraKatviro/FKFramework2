#ifndef FKABSTRACTCORE_H
#define FKABSTRACTCORE_H

#include <QObject>
#include <QMap>
#include <QStringList>

#include "fkcore_global.h"

#include "FKFilesList.h"

class FKRealm;
class FKServerInfrastructure;
class FKClientInfrastructure;
enum class FKInfrastructureType:qint8;

class FKCORESHARED_EXPORT FKAbstractCore:public QObject{
    Q_OBJECT
    Q_PROPERTY(bool waitingForRealmAnswer READ waitingForRealmAnswer NOTIFY waitingForRealmAnswerChanged)
    Q_PROPERTY(bool waitingForServerAnswer READ waitingForServerAnswer NOTIFY waitingForServerAnswerChanged)
    Q_PROPERTY(QStringList userPool READ getUserPool NOTIFY userPoolChanged)
    Q_PROPERTY(QStringList userList READ getUserList NOTIFY userListChanged)
public:
    FKAbstractCore(QObject* parent=0);
    ~FKAbstractCore();
signals:
    void messageRequested(const QString msg);
    void systemMessageRequested(const QString msg);
    void clientConnectedToRealm();
    void clientDisconnectedFromRealm();
    void clientLoggedIn();
    void serverConnectedToRealm();
    void serverDisconnectedFromRealm();
    void serverLoggedIn();
    void customServerReady(qint32 serverId);
    void waitingForRealmAnswerChanged();
    void waitingForServerAnswerChanged();
    void userPoolChanged();
    void userListChanged();
    void realmStarted();
public slots:
    virtual bool startRealm(const int port=0)=0;
    virtual bool startServer(const int port=0, const int realmPort=0,const QString& realmIP=QString())=0;
    virtual bool startClientInfrastructure(const int realmPort=0,const QString& realmIP=QString())=0;
    void ausviseClientInfrastructure(const QString id, const QString password);
    void ausviseServerInfrastructure(const qint32 id, const QString password);
    bool createUserRecord(const QString& name);
    bool deleteUserRecord(const QString& name);
    bool startUser(const QString& name);
    bool stopUser(const QString& name);
    virtual bool createRoom(const QMap<QString,QVariant>& roomData)=0;
    virtual bool joinRoom(const qint32 id)=0;

    virtual bool stopRealm()=0;
    virtual bool stopServer()=0;
    virtual bool stopClientManager()=0;
    virtual bool quitRoom()=0;

    void setApplicationFilesList(){_appFilesList=buildApplicationFilesList();}
    const FKFilesList& applicationFilesList()const{return _appFilesList;}

    void quitApplication();
    void restartApplication();

    bool waitingForRealmAnswer()const;
    bool waitingForServerAnswer()const;
    QStringList getUserPool()const;
    QStringList getUserList()const;
    QStringList realmUserList()const;

    void createClientRecord(const QString clientName, const QString password);
    void createServerRecord(const QString password);
    void registerRoomType(const QString roomType);
    void registerServerRoomType(const QString roomType);

    void createRoomRequest(const QString roomName, const QString roomType);
    void createRoomRequestRealm(const QString roomName, const QString roomType);
    void createCustomServerRequest();
protected:
    void addPostExecutionCommand(const QString& command);
    virtual FKFilesList buildApplicationFilesList()const;

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
    void createCustomServer(const qint32 id,const QString password);
    void serverConnectedToRealmSlot();
    void serverDisconnectedFromRealmSlot();
    void serverLoggedInSlot();
private:
    virtual qint32 customServerPort()const;
    FKFilesList _appFilesList;

    FKRealm* _realm;
    FKServerInfrastructure* _server;
    FKClientInfrastructure* _infr;

    qint32 _customServerId;
    QString _customServerPassword;
};



#endif // FKABSTRACTCORE_H
