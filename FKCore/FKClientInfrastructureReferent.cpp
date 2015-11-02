#include "FKClientInfrastructureReferent.h"

#include "FKServerConnectionManager.h"
#include "FKEventObject.h"

#include <QTimer>

#include "FKLogger.h"

/*!
\class FKClientInfrastructureReferent
\brief Class incapsulates server-side client connection manager and set of users from that client to manage event delivery
*/

FKClientInfrastructureReferent::FKClientInfrastructureReferent(const QString& clientId, const QString& password):
        _id(clientId),_password(password){
    FK_CBEGIN
    const int inviteTime=1000*16; //16 seconds to create indentified client-server connection
    QTimer::singleShot(inviteTime,this,&FKClientInfrastructureReferent::inviteTimeout);
    FK_CEND
}

FKClientInfrastructureReferent::~FKClientInfrastructureReferent(){
    FK_DBEGIN
    FK_DEND
}

void FKClientInfrastructureReferent::createConnectionManager(FKConnector* connector){
    _connection=new FKServerConnectionManagerU(this,connector,this);
}

void FKClientInfrastructureReferent::setPassword(const QString& password){
    _password=password;
}

bool FKClientInfrastructureReferent::isActive() const{
    return _invited && _connected;
}

bool FKClientInfrastructureReferent::isInvited() const{
    return _invited;
}

bool FKClientInfrastructureReferent::isConnected() const{
    return _connected;
}

void FKClientInfrastructureReferent::setInvited(){
    _invited=true;
}

void FKClientInfrastructureReferent::setConnected(FKConnector* connector){
    _connected=true;
    createConnectionManager(connector);
}

void FKClientInfrastructureReferent::addUser(const qint32 userId){
    _users.append(userId);
}

void FKClientInfrastructureReferent::dropConnection(){
    todo;
}

QList<qint32> FKClientInfrastructureReferent::users() const{
    return _users;
}

QString FKClientInfrastructureReferent::password() const{
    return _password;
}

void FKClientInfrastructureReferent::sendBasicEvent(FKBasicEvent* ev){
    _connection->sendBasicEvent(ev);
}

void FKClientInfrastructureReferent::pushEvent(FKEventObject* ev){
    if(!_event){
        _event=static_cast<FKEventObject*>(ev->clone());
    }
}

void FKClientInfrastructureReferent::sendEvent(){
    if(_event){
        _connection->sendEvent(_event);
        _event->deleteLater();
        _event=nullptr;
    }
}

void FKClientInfrastructureReferent::incomeAction(FKEventObject* ev){
    if(_users.contains(ev->client())){
        emit actionRequested(ev);
    }else{
        ev->deleteLater();
        FK_MLOGV("Client falsification attempt by client",_id)
    }
}



