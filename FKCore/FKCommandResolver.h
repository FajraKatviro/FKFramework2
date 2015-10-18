#ifndef FKCOMMANDRESOLVER_H
#define FKCOMMANDRESOLVER_H

#include <QObject>

#include "fkcore_global.h"

class FKCORESHARED_EXPORT FKCommandResolver : public QObject{
    Q_OBJECT
public:
    explicit FKCommandResolver(QObject *parent = 0);
    ~FKCommandResolver();
public slots:
    void processInput(QString input);
signals:
    void message(const QString msg);
    void quitApplicationRequested();
    void startRealmRequested();
    void stopRealmRequested();
    void startClientInfrastructureRequested();
    void startServerInfrastructureRequested();
    void stopServerInfrastructureRequested();
    void clientUsersRequested();
    void realmUsersRequested();
    void clientLoginRequested(const QString client,const QString password);
    void serverLoginRequested(const qint32 server,const QString password);
    void createClientRequested(const QString client,const QString password);
    void deleteClientRequested(const QString client);
    void createServerRequested(const QString password);
    void deleteServerRequested(const qint32 server);
    void createRoomRequestedRealm(const QString roomName,const QString roomType);
    void createRoomRequested(const QString roomName,const QString roomType,const QStringList users);
    void joinRoomRequested(const QString roomName,const QStringList users);
    void registerServerRoomTypeRequested(const QString roomType);
    void registerRoomTypeRequested(const QString roomType);
    void removeServerRoomTypeRequested(const QString roomType);
    void removeRoomTypeRequested(const QString roomType);
    void showServerRoomTypesRequested();
    void showRoomTypesRequested(const qint32 serverId);
    void showServersRequested(const QString roomType);
    void showClientsRequested();
private:
    QString welcomeString()const;
    void printHelp();
    static bool isCommand(QString& arg, const QString& command,const bool exactMatch=false);
    static bool hasKey(QString& arg, const QString& key);
    void showUsers(QString& arg);
    void loginClient(const QString& arg);
    void loginServer(const QString& arg);
    void createClient(const QString& arg);
    void deleteClient(const QString& arg);
    void createServer(QString arg);
    void deleteServer(QString arg);
    void createRoom(QString arg);
    void joinRoom(QString arg);
    void registerRoomType(QString arg);
    void removeRoomType(QString arg);
    void showRoomTypes(QString arg);
    void showServers(QString arg);
};

#endif // FKCOMMANDRESOLVER_H
