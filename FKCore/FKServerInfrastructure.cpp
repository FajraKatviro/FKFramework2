#include "FKServerInfrastructure.h"

#include "FKConnectionManager.h"
#include "FKServerInfrastructureConnectionManager.h"
#include "FKBasicEvent.h"

#include "FKAusviceIdentifiers.h"
#include "FKBasicEventSubjects.h"
#include "FKLogger.h"

FKClientInfrastructure::FKClientInfrastructure(QObject *parent):
        FKInfrastructure(parent),_logged(false),_realmConnection(0){
    FK_CBEGIN
    FK_CEND
}

FKClientInfrastructure::~FKClientInfrastructure(){
    FK_DBEGIN
    FK_DEND
}

FKInfrastructureType FKClientInfrastructure::infrastructureType() const{
    return FKInfrastructureType::Client;
}

bool FKClientInfrastructure::isConnectedToRealm(){
    return _realmConnection && _realmConnection->isActive();
}

void FKClientInfrastructure::dropInfrastructure(){
    for(auto i=_users.begin();i!=_users.end();++i){
        //i.value()->dropUser();
        //i.value()->deleteLater();
    }
    _users.clear();
    _realmConnection->dropConnection();
    _realmConnection->deleteLater();
    _realmConnection=0;
    _logged=false;
    cancelAnswer(FKInfrastructureType::Realm);
}

void FKClientInfrastructure::requestLoginRealm(const QString& id, const QString& password){
    if(id.isEmpty() || password.isEmpty()){
        emit messageRequested(QString(tr("Unable login: empty id or password")));
        return;
    }

    if(!isConnectedToRealm()){
        emit messageRequested(QString(tr("Unable login: not connected to realm")));
        return;
    }

    if(_logged){
        emit messageRequested(QString(tr("Unable login: already logged")));
        return;
    }

    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::login)){
        emit messageRequested(QString(tr("Unable login: another request in progress")));
        return;
    }

    QMap<QString,QVariant> data;
    data[FKAusviceIdentifiers::infrastructureType]=FKAusviceIdentifiers::client;
    data[FKAusviceIdentifiers::id]=id;
    data[FKAusviceIdentifiers::password]=password;
    FKBasicEvent ev(FKBasicEventSubject::login,data);
    _realmConnection->sendGuestEvent(&ev);
}

void FKClientInfrastructure::submitLoginRealm(const QVariant& value){
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::login)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::submitLoginRealm()")
    }
    bool t=value.toBool();
    if(t){
        _logged=true;
        emit loggedIn();
    }else{
        emit messageRequested(QString(tr("Realm declined login command")));
    }
}

void FKClientInfrastructure::requestUserCreation(const QString& name){
    if(name.isEmpty()){
        emit messageRequested(QString(tr("Unable create user: name is empty")));
        return;
    }
    if(!_logged){
        emit messageRequested(QString(tr("Unable create user: infrastructure is not logged on realm")));
        return;
    }
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::createUser)){
        emit messageRequested(QString(tr("Unable create user: another request in progress")));
        return;
    }
    FKBasicEvent ev(FKBasicEventSubject::createUser,name);
    _realmConnection->sendBasicEvent(&ev);
}

void FKClientInfrastructure::requestUserDeletion(const QString& name){
    if(name.isEmpty()){
        emit messageRequested(QString(tr("Unable delete user: name is empty")));
        return;
    }
    if(!_logged){
        emit messageRequested(QString(tr("Unable delete user: infrastructure is not logged on realm")));
        return;
    }
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::deleteUser)){
        emit messageRequested(QString(tr("Unable delete user: another request in progress")));
        return;
    }
    FKBasicEvent ev(FKBasicEventSubject::deleteUser,name);
    _realmConnection->sendBasicEvent(&ev);
}

void FKClientInfrastructure::requestUserAuthorization(const QString& name){
    if(name.isEmpty()){
        emit messageRequested(QString(tr("Unable select user: name is empty")));
        return;
    }
    if(!_logged){
        emit messageRequested(QString(tr("Unable select user: infrastructure is not logged on realm")));
        return;
    }
    if(!_userPool.contains(name)){
        emit messageRequested(QString(tr("Unable select user: no such name in user pool")));
        return;
    }
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::selectUser)){
        emit messageRequested(QString(tr("Unable select user: another request in progress")));
        return;
    }
    FKBasicEvent ev(FKBasicEventSubject::selectUser,name);
    _realmConnection->sendBasicEvent(&ev);
}

void FKClientInfrastructure::requestUserDeauthorization(const QString& name){
    if(name.isEmpty()){
        emit messageRequested(QString(tr("Unable deselect user: name is empty")));
        return;
    }
    if(!_logged){
        emit messageRequested(QString(tr("Unable deselect user: infrastructure is not logged on realm")));
        return;
    }
    if(!activeUsers().contains(name)){
        emit messageRequested(QString(tr("Unable deselect user: no such user selected")));
        return;
    }
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::deselectUser)){
        emit messageRequested(QString(tr("Unable deselect user: another request in progress")));
        return;
    }
    FKBasicEvent ev(FKBasicEventSubject::deselectUser,name);
    _realmConnection->sendBasicEvent(&ev);
}

void FKClientInfrastructure::requestRoomList(/*filters*/){
    if(!_logged){
        emit messageRequested(QString(tr("Unable request room list: infrastructure is not logged on realm")));
        return;
    }
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::roomList)){
        emit messageRequested(QString(tr("Unable request room list: another request in progress")));
        return;
    }
    QVariant filters;
    FKBasicEvent ev(FKBasicEventSubject::roomList,filters);
    _realmConnection->sendBasicEvent(&ev);
}

void FKClientInfrastructure::refreshUserList(const QVariant& value){
    QStringList lst(value.toStringList());
    foreach(QString u,_users.keys())lst.removeOne(u);
    _userPool=lst;
    emit userPoolChanged();
}

void FKClientInfrastructure::respondUserCreation(const QVariant& value){
    const QString name=value.toString();
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::createUser)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::respondUserCreation()")
    }
    if(!name.isEmpty()){
        _userPool.append(name);
        emit userPoolChanged();
    }
}

void FKClientInfrastructure::respondUserDeletion(const QVariant& value){
    const QString name=value.toString();
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::deleteUser)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::respondUserDeletion()")
    }
    if(!name.isEmpty()){
        _userPool.removeOne(name);
        emit userPoolChanged();
    }
}

void FKClientInfrastructure::respondUserAuthorization(const QVariant& value){
    const QString name=value.toString();
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::selectUser)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::respondUserAuthorization()")
    }
    if(!name.isEmpty()){
        _userPool.removeOne(name);
        //_users.insert(name,new FKUserInfrastructure(this));
        emit userPoolChanged();
        emit activeUsersChanged();
    }
}

void FKClientInfrastructure::respondUserDeauthorization(const QVariant& value){
    const QString name=value.toString();
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::deselectUser)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::respondUserDeauthorization()")
    }
    if(!name.isEmpty()){
        _userPool.append(name);
        FKUserInfrastructure* user=_users.take(name);
        //user->dropInfrastructure();
        //user->deleteLater();
        emit userPoolChanged();
        emit activeUsersChanged();
    }
}

void FKClientInfrastructure::respondRoomList(const QVariant& value){
    if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::roomList)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::respondRoomList()")
    }
    _roomList=value.toStringList();
    emit roomListChanged();
}

QStringList FKClientInfrastructure::userPool() const{
    return _userPool;
}

QStringList FKClientInfrastructure::activeUsers() const{
    return _users.keys();
}

void FKClientInfrastructure::messageFromRealm(const QString& msg){
    emit messageRequested(QString(tr("Realm -> client: %1")).arg(msg));
}

void FKClientInfrastructure::realmConnection(FKConnector* connector){
    _realmConnection=new FKClientInfrastructureConnectionManager(this,connector,this);
    connect(_realmConnection,SIGNAL(connectionStatusChanged()),SLOT(connectorStatusChanged()));
    if(!requestAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::connect)){
        FK_MLOG("Unexpected behaivour in FKClientInfrastructure::realmConnection()")
    }
}

void FKClientInfrastructure::connectorStatusChanged(){
    if(_realmConnection->isActive()){
        emit connectedToRealm();
        if(!submitAnswer(FKInfrastructureType::Realm,FKBasicEventSubject::connect)){
            FK_MLOG("Unexpected behaivour in FKClientInfrastructure::connectorStatusChanged()")
        }
    }else{
        _logged=false;
        emit disconnectedFromRealm();
        cancelAnswer(FKInfrastructureType::Realm);
    }
}

/*!
 * \fn void FKClientInfrastructure::connectedToRealm()
 * \brief Signal emitted when infrastructure successfully connects as guest to realm
 */

/*!
 * \fn void FKClientInfrastructure::disconnectedFromRealm()
 * \brief Signal emitted when infrastructure disconnects from realm
 */

/*!
 * \fn void FKClientInfrastructure::loggedIn()
 * \brief Signal emitted when infrastructure recieve ausvice submit from realm
 */


