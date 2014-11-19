#include "FKUserInfrastructureSlot.h"

#include "FKBasicEventSubjects.h"
#include "FKLogger.h"

FKUserInfrastructureSlot::FKUserInfrastructureSlot(FKClientInfrastructureSlot *client, const qint32 id, const QString &password, QObject *parent):
        FKInfrastructure(parent),_id(id),_password(password),_connection(0),_client(client){
    FK_CBEGIN
    FK_CEND
}

FKUserInfrastructureSlot::~FKUserInfrastructureSlot(){
    FK_DBEGIN
    FK_DEND
}

FKInfrastructureType FKUserInfrastructureSlot::infrastructureType() const{
    return FKInfrastructureType::UserSlot;
}

QString FKUserInfrastructureSlot::password() const{
    return _password;
}

bool FKUserInfrastructureSlot::isActive() const{
    return _connection && _connection->isActive();
}

void FKUserInfrastructureSlot::dropUser(){
    if(_connection){
        _connection->dropConnection();
        _connection->deleteLater();
        _connection=0;
        cancelAnswer(FKInfrastructureType::User);
    }
}

void FKUserInfrastructureSlot::setUserConnector(FKConnector* connector){
    _connection=new FKServerInfrastructureConnectionManagerU(this,connector,this);
    connect(_connection,SIGNAL(connectionStatusChanged()),SLOT(connectorStatusChanged()));
}
