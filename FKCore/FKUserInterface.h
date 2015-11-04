#ifndef FKUSERINTERFACE_H
#define FKUSERINTERFACE_H

#include "fkcore_global.h"

#include <QObject>

class FKCommandResolver;

class FKCORESHARED_EXPORT FKUserInterface:public QObject{
    Q_OBJECT
public:
    FKUserInterface(QObject* parent);
    ~FKUserInterface();
    virtual void installCommandsResolver(FKCommandResolver* resolver);
    FKCommandResolver* commandResolver()const;
public slots:
    virtual void showMessage(const QString& m)=0;
    virtual void show()=0;
    virtual void showUpdateData(){}
signals:
    void quitApplicationRequested();
    void showMessageRequested(QString msg);
    void startRealmRequested();
    void stopRealmRequested();
    void startClientInfrastructureRequested();
    void stopClientInfrastructureRequested();
    void startServerInfrastructureRequested();
    void stopServerInfrastructureRequested();
    void dropServerRequested(const qint32 serverId);
    void createClientRequested(const QString clientName,const QString password);
    void deleteClientRequested(const QString clientName);
    void createRoomRequested(const QString roomName,const QString roomType, const QStringList users);
    void joinRoomRequested(const QString roomName, const QStringList users);
    void createRoomRequestedRealm(const QString roomName,const QString roomType);
    void createServerRequested(const QString password);
    void deleteServerRequested(const qint32 serverId);
    //void createCustomServerRequested();
    void clientLoginRequested(const QString clientName,const QString password);
    void serverLoginRequested(const qint32 serverId,const QString password);
    void createUserRequested(const QString userName);
    void deleteUserRequested(const QString userName);
    void registerRoomTypeRequested(const QString roomType);
    void registerServerRoomTypeRequested(const QString roomType);
    void removeRoomTypeRequested(const QString roomType);
    void removeServerRoomTypeRequested(const QString roomType);
    void showRoomTypesRequested(const qint32 serverId);
    void showServerRoomTypesRequested();
    void showServersRequested(const QString roomType,bool onlineOnly,bool avaliableOnly);
    void showClientsRequested();
    void clientUsersRequested();
    void realmUsersRequested();
protected:
    void requestCreateUser(const QString& arg);
    void requestDeleteUser(const QString& arg);
private:
    FKCommandResolver* _commandResolver=nullptr;
};

#endif
