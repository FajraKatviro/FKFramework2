#include "FKLocalCore.h"

#include "FKRealm.h"
#include "FKServerInfrastructure.h"
#include "FKClientInfrastructure.h"
#include "FKLocalConnector.h"

#include "FKLogger.h"

/*!
 * \class FKLocalCore
 * \brief Local core provides full functionality for managing objects, exept of network data transmitting
 */

/*!
 * \brief Creates local core. Usually, instance have to be created by QML automaticly.
 */

FKLocalCore::FKLocalCore():FKAbstractCore(){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Destructs local core
 */

FKLocalCore::~FKLocalCore(){
    FK_DBEGIN
    FK_DEND
}

bool FKLocalCore::startRealm(const int){
    if(realm()){
        emit messageRequested(QString(tr("Unable start realm becouse it is already started")));
        return false;
    }
    setRealm(new FKRealm(this));
    emit systemMessageRequested(QString(tr("Realm started")));
    emit realmStarted();
    return true;
}

bool FKLocalCore::startServer(const int, const int, const QString&){
    if(server()){
        emit messageRequested(QString(tr("Unable start server becouse it is already started")));
        return false;
    }
    if(!realm()){//this check for local core only
        emit messageRequested(QString(tr("Unable start server: no realm started")));
        return false;
    }
    setServer(new FKServerInfrastructure(this));
    FKLocalConnector* realmSideConnector=new FKLocalConnector(realm());
    FKLocalConnector* serverSideConnector=new FKLocalConnector(server());
    realm()->incomeConnection(realmSideConnector);
    server()->realmConnection(serverSideConnector);
    realmSideConnector->join(serverSideConnector);
    emit systemMessageRequested(QString(tr("Server started")));
    return true;
}

bool FKLocalCore::startClientInfrastructure(const int, const QString&){
    if(clientInfrastructure()){
        emit messageRequested(QString(tr("Unable start client infrastructure becouse it is already started")));
        return false;
    }
    if(!realm()){
        emit messageRequested(QString(tr("Unable start client infrastructure: target realm does not exists")));
        return false;
    }
    setClientInfrastructure(new FKClientInfrastructure(this));
    FKLocalConnector* realmSideConnector=new FKLocalConnector(realm());
    FKLocalConnector* clientSideConnector=new FKLocalConnector(clientInfrastructure());
    realm()->incomeConnection(realmSideConnector);
    clientInfrastructure()->realmConnection(clientSideConnector);
    realmSideConnector->join(clientSideConnector);
    emit systemMessageRequested(QString(tr("Client infrastructure started")));
    return true;
}

bool FKLocalCore::stopRealm(){
    realm()->dropRealm();
    realm()->deleteLater();
    return true;
}

void FKLocalCore::connectClientToServer(const QString address, const qint32 port){
    Q_UNUSED(address)
    Q_UNUSED(port)
    if(!clientInfrastructure()){
        emit messageRequested(QString(tr("Warning! Unable start user: no client started")));
        return;
    }
    if(!server()){
        emit messageRequested(QString(tr("Unable start user: no server started")));
        return;
    }

    emit systemMessageRequested(QString(tr("Client connecting to server..")));
    FKLocalConnector* clientSideConnector=new FKLocalConnector(clientInfrastructure());
    FKLocalConnector* serverSideConnector=new FKLocalConnector(server());
    server()->clientConnection(serverSideConnector);
    clientInfrastructure()->serverConnection(clientSideConnector);
    serverSideConnector->join(clientSideConnector);
}
