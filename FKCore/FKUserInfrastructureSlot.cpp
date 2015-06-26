#include "FKUserInfrastructureSlot.h"

#include "FKServerConnectionManager.h"
#include "FKEventObject.h"

#include "FKLogger.h"

FKUserInfrastructureSlot::FKUserInfrastructureSlot(FKUserInfrastructureAlias* alias,const qint32 userObjectId,const QString& invitePassword):
        QObject(),_id(userObjectId),_password(invitePassword),_connection(0),_client(alias),_active(false){
    FK_CBEGIN
    FK_CEND
}

FKUserInfrastructureSlot::~FKUserInfrastructureSlot(){
    FK_DBEGIN
    FK_DEND
}

QString FKUserInfrastructureSlot::password() const{
    return _password;
}

bool FKUserInfrastructureSlot::isActive() const{
    return _active;
}

void FKUserInfrastructureSlot::setActive(const bool t){
    _active=t;
}

void FKUserInfrastructureSlot::dropUser(){
    if(_connection){
        _connection->dropConnection();
        _connection->deleteLater();
        _connection=0;
        _active=false;
    }
}

void FKUserInfrastructureSlot::setUserConnector(FKConnector* connector){
    _connection=new FKServerConnectionManagerU(this,connector,this);
}

void FKUserInfrastructureSlot::sendMessage(const QString msg){
    _connection->sendMessage(msg);
}

void FKUserInfrastructureSlot::sendEvent(FKEventObject* event){
    _connection->sendEvent(event);
}

void FKUserInfrastructureSlot::sendData(const QByteArray& data){
    _connection->sendData(data);
}

void FKUserInfrastructureSlot::incomeAction(FKEventObject* action){
    if(_client->isActive()){
        emit gotAction(action);
    }else{
        action->deleteLater();
    }
}


FKUserInfrastructureAlias::FKUserInfrastructureAlias(const QString& clientId):_id(clientId){}

FKUserInfrastructureAlias::FKUserInfrastructureAlias(const FKUserInfrastructureAlias& other)
    :_id(other._id),_users(other._users){}

FKUserInfrastructureAlias::~FKUserInfrastructureAlias(){
    dropAlias();
}

bool FKUserInfrastructureAlias::isActive() const{
    for(auto i=_users.constBegin();i!=_users.constEnd();++i){
        if(!(*i)->isActive())return false;
    }
    return true;
}

void FKUserInfrastructureAlias::addUser(FKUserInfrastructureSlot* user){
    _users.append(user);
}

void FKUserInfrastructureAlias::dropAlias(){
    for(auto i=_users.constBegin();i!=_users.constEnd();++i){
        (*i)->dropUser();
        (*i)->deleteLater();
    }
}

