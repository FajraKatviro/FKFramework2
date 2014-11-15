#include "FKRealm.h"

#include "FKRealmConnectionManager.h"
#include "FKDataBase.h"
#include "FKBasicEvent.h"

#include "FKAusviceIdentifiers.h"
#include "FKBasicEventSubjects.h"
#include "FKLogger.h"

const QString FKRealm::clientBranch=QString("clients");
const QString FKRealm::serverBranch=QString("servers");
const QString FKRealm::userBranch=QString("users");
const QString FKRealm::usersNode=QString("users");
const QString FKRealm::roomNameNode=QString("roomName");
const qint32 FKRealm::userSelected=1;
const qint32 FKRealm::userNotSelected=2;

/*!
\class FKRealm
\brief This class represents realm infrastructure. Realm manages multiple other infrastructures and make login validation.
*/

/*!
 * \brief Creates realm object with given parent
 */

FKRealm::FKRealm(QObject *parent):FKInfrastructure(parent),_db(0){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Deletes realm object
 */

FKRealm::~FKRealm(){
    FK_DBEGIN
    FK_DEND
}

/*!
 * \brief Process guest login request. Validate income data and replace guest manager with client or server manager on success.
 */

void FKRealm::ausvise(FKConnectionManager* source, const QVariant loginData){
    if(!source){
        FK_MLOG("Empty source connection manager passed to realm ausvice")
        return;
    }
    
    QMap<QString,QVariant> data=loginData.toMap();
    if(data.isEmpty()){
        FK_MLOGV("Invalid login data container recived from guest",loginData)
        stopGuestConnection(source);
        return;
    }
    
    const QString sourceType=data.take(FKAusviceIdentifiers::infrastructureType).toString();
    if(sourceType!=FKAusviceIdentifiers::client && sourceType!=FKAusviceIdentifiers::server){
        FK_MLOGV("Invalid ausvice identifier recived from guest",data.value(FKAusviceIdentifiers::infrastructureType))
        stopGuestConnection(source);
        return;
    }
    
    const QString password=data.take(FKAusviceIdentifiers::password).toString();
    if(password.isEmpty()){
        FK_MLOG("Password for ausvice not provided by guest")
        stopGuestConnection(source);
        return;
    }
    
    const QVariant id=data.take(FKAusviceIdentifiers::id);
    if(!data.isEmpty()){
        FK_MLOGV("Too many arguments for ausvice provided by guest",data)
        stopGuestConnection(source);
        return;
    }
    
    if(sourceType==FKAusviceIdentifiers::client){
        const QString strId=id.toString();
        if(strId.isEmpty()){
            FK_MLOGV("Invalid client id provided by guest",id)
            stopGuestConnection(source);
            return;
        }

        if(!isClientId(strId)){
            FK_MLOGV("Invalid client name provided by guest",strId)
            stopGuestConnection(source);
            return;
        }

        if(isClientConnected(strId)){
            FK_MLOGV("Requested client already connected",strId)
            stopGuestConnection(source);
            return;
        }

        const QString clientPassword=getClientPassword(strId);
        if(clientPassword.isEmpty()){
            FK_MLOGV("Client id provided by guest is not registered",strId)
            FKBasicEvent deny(FKBasicEventSubject::login,false);
            source->sendBasicEvent(&deny);
            const QString message(tr("Invalid client id"));
            source->sendMessage(message);
            return;
        }

        if(clientPassword!=password){
            FK_MLOGV("Client password provided by guest does not match",clientPassword)
            FKBasicEvent deny(FKBasicEventSubject::login,false);
            source->sendBasicEvent(&deny);
            const QString message(tr("Invalid client password"));
            source->sendMessage(message);
            return;
        }

        incomeClient(strId,source->connector());

    }else{
        bool test;
        const qint32 intId=id.toInt(&test);
        if(!test){
            FK_MLOGV("Invalid server id provided by guest",id)
            stopGuestConnection(source);
            return;
        }

        if(isServerConnected(intId)){
            FK_MLOGV("Requested server already started",intId)
            stopGuestConnection(source);
            return;
        }

        const QString serverPassword=getServerPassword(intId);
        if(serverPassword.isEmpty()){
            FK_MLOGV("Server id provided by guest is not registered",intId)
            FKBasicEvent deny(FKBasicEventSubject::login,false);
            source->sendBasicEvent(&deny);
            const QString message(tr("Invalid server id"));
            source->sendMessage(message);
            return;
        }

        if(serverPassword!=password){
            FK_MLOGV("Server password provided by guest does not match",serverPassword)
            FKBasicEvent deny(FKBasicEventSubject::login,false);
            source->sendBasicEvent(&deny);
            const QString message(tr("Invalid server password"));
            source->sendMessage(message);
            return;
        }

        incomeServer(intId,source->connector());
    }
    _guesConnections.remove(source);
    source->deleteLater();
}

/*!
 * \brief Drop guest connection, remove manager and delete it
 */

void FKRealm::stopGuestConnection(FKConnectionManager* guest){
    if(_guesConnections.remove(guest)){
        guest->dropConnection();
        guest->deleteLater();
        emit messageRequested(QString(tr("Guest connection stopped")));
    }else{
        FK_MLOG("Failed stop guest connection")
    }
}

/*!
 * \brief Drop server connection, remove manager and delete it
 */

void FKRealm::stopServerConnection(const qint32 serverId){
    FKConnectionManager* server=_serverConnections.value(serverId,0);
    if(server){
        _serverConnections.remove(serverId);
        server->dropConnection();
        server->deleteLater();
        emit messageRequested(QString(tr("Server %1 connection stopped")).arg(QString(serverId)));
    }else{
        FK_MLOGV("Failed stop server connection (not found)",serverId)
    }
}

/*!
 * \brief Drop client connection, remove manager and delete it
 */

void FKRealm::stopClientConnection(const QString& clientId){
    FKConnectionManager* client=_clientConnections.value(clientId,0);
    if(client){
        _clientConnections.remove(clientId);
        client->dropConnection();
        client->deleteLater();
        emit messageRequested(QString(tr("Client %1 connection stopped")).arg(clientId));
    }else{
        FK_MLOGV("Failed stop client connection (not found)",clientId)
    }
}

/*!
 * \brief Respond client request: create list of avaliable rooms, apply \i filters and send to client.
 */

void FKRealm::refreshRoomList(const QString& clientId, const QVariant& filters){
    auto rooms=getRoomList(filters);
    FKConnectionManager* mgr=_clientConnections.value(clientId);
    FKBasicEvent ev(FKBasicEventSubject::roomList,rooms);
    mgr->sendBasicEvent(&ev);
    emit messageRequested(QString(tr("Room list sent to %1 client")).arg(clientId));
}

/*!
 * \brief Respond server request: update server's room info
 */

void FKRealm::refreshRoomData(const qint32 serverId, const QVariant& data){

}

/*!
 * \brief Set database containing clients' and servers' login data. Database must be set before any incomeConnection() call
 */

void FKRealm::setDataBase(FKDataBase* db){
    _db=db;
}

/*!
 * \brief Disconnect all connected infrastructures
 */

void FKRealm::dropRealm(){
    foreach(FKConnectionManager* guest,_guesConnections){
        stopGuestConnection(guest);
    }
    foreach(QString client,_clientConnections.keys()){
        stopClientConnection(client);
    }
    foreach(qint32 server,_serverConnections.keys()){
        stopServerConnection(server);
    }
}

/*!
 * \brief This is reimplemented function
 */

FKInfrastructureType FKRealm::infrastructureType() const{
    return FKInfrastructureType::Realm;
}

/*!
 * \brief Try create new client record with given parametrs
 */

void FKRealm::createClientRecord(const QString& clientName, const QString& password){
    if(password.isEmpty()){
        emit messageRequested(QString(tr("Unable create client: empty password provided")));
        return;
    }
    if(clientName.isEmpty()){
        emit messageRequested(QString(tr("Unable create client: empty name provided")));
        return;
    }
    if(!isClientId(clientName)){
        emit messageRequested(QString(tr("Unable create client: invalid name provided")));
        return;
    }
    if(isClientExists(clientName)){
        emit messageRequested(QString(tr("Unable create client: name already in use")));
        return;
    }

    createNewClientRecord(clientName,password);
    emit messageRequested(QString(tr("Client record creation success")));
}

/*!
 * \brief Try create user for given client and send reply
 */

void FKRealm::createUser(const QString& clientId, const QVariant& userName){
    QString errorMessage;
    QString name=userName.toString();
    if(name.isEmpty()){
        errorMessage=QString(tr("Unable create user: invalid argument"));
    }else if(!isUserName(name)){
        errorMessage=QString(tr("Unable create user: invalid username"));
    }else if(isUserExists(name)){
        errorMessage=QString(tr("Unable create user: username already in use"));
    }

    FKConnectionManager* mgr=_clientConnections.value(clientId);
    if(errorMessage.isEmpty()){
        createUser(clientId,name);
    }else{
        name.clear();
        mgr->sendMessage(errorMessage);
    }
    FKBasicEvent ev(FKBasicEventSubject::createUser,name);
    mgr->sendBasicEvent(&ev);
}

/*!
 * \brief Try delete user for given client and send reply
 */

void FKRealm::deleteUser(const QString& clientId, const QVariant& userName){
    QString errorMessage;
    QString name=userName.toString();
    if(name.isEmpty()){
        errorMessage=QString(tr("Unable delete user: invalid argument"));
    }else if(!isUserName(name)){
        errorMessage=QString(tr("Unable delete user: invalid username"));
    }else if(!isUserExists(name,clientId)){
        errorMessage=QString(tr("Unable delete user: no such user found"));
    }else if(getUserStatus(clientId,name)!=userNotSelected){
        errorMessage=QString(tr("Unable delete user. Please check user not active"));
    }

    FKConnectionManager* mgr=_clientConnections.value(clientId);
    if(errorMessage.isEmpty()){
        deleteUser(clientId,name);
    }else{
        name.clear();
        mgr->sendMessage(errorMessage);
    }
    FKBasicEvent ev(FKBasicEventSubject::deleteUser,name);
    mgr->sendBasicEvent(&ev);
}

/*!
 * \brief Try select given user for given client
 */

void FKRealm::selectUser(const QString& clientId, const QVariant& userName){
    QString errorMessage;
    QString name=userName.toString();
    if(name.isEmpty()){
        errorMessage=QString(tr("Unable select user: invalid argument"));
    }else if(!isUserName(name)){
        errorMessage=QString(tr("Unable select user: invalid username"));
    }else if(!isUserExists(name,clientId)){
        errorMessage=QString(tr("Unable select user: no such user found"));
    }else if(getUserStatus(clientId,name)!=userNotSelected){
        errorMessage=QString(tr("Unable select user. Please check user not active"));
    }

    FKConnectionManager* mgr=_clientConnections.value(clientId);
    if(errorMessage.isEmpty()){
        selectUser(clientId,name);
    }else{
        name.clear();
        mgr->sendMessage(errorMessage);
    }
    FKBasicEvent ev(FKBasicEventSubject::selectUser,name);
    mgr->sendBasicEvent(&ev);
}

/*!
 * \brief Try deselect given user for given client
 */

void FKRealm::deselectUser(const QString& clientId, const QVariant& userName){
    QString errorMessage;
    QString name=userName.toString();
    if(name.isEmpty()){
        errorMessage=QString(tr("Unable deselect user: invalid argument"));
    }else if(!isUserName(name)){
        errorMessage=QString(tr("Unable deselect user: invalid username"));
    }else if(!isUserExists(name,clientId)){
        errorMessage=QString(tr("Unable deselect user: no such user found"));
    }else if(getUserStatus(clientId,name)!=userSelected){
        errorMessage=QString(tr("Unable deselect user. Please check user is active"));
    }

    FKConnectionManager* mgr=_clientConnections.value(clientId);
    if(errorMessage.isEmpty()){
        deselectUser(clientId,name);
    }else{
        name.clear();
        mgr->sendMessage(errorMessage);
    }
    FKBasicEvent ev(FKBasicEventSubject::deselectUser,name);
    mgr->sendBasicEvent(&ev);
}

/*!
 * \brief Return list of users registered in database
 */

QStringList FKRealm::userList() const{
    FKDBIndex ind;
    ind>>userBranch;
    return _db->getProperties(ind);
}

void FKRealm::createUser(const QString& clientId, const QString& userName){
    FKDBIndex clientInd, userInd;
    clientInd>>clientBranch>>clientId>>usersNode>>userName;
    userInd>>userBranch>>userName;
    _db->writeValue(FKDBValue(userNotSelected),clientInd,false);
    _db->writeValue(FKDBValue(clientId),userInd,false);
    emit messageRequested(QString(tr("New user %2 created for %1 client")).arg(clientId).arg(userName));
}

void FKRealm::deleteUser(const QString& clientId, const QString& userName){
    FKDBIndex clientInd, userInd;
    clientInd>>clientBranch>>clientId>>usersNode>>userName;
    userInd>>userBranch>>userName;
    _db->removeIndex(clientInd);
    _db->removeIndex(userInd);
    emit messageRequested(QString(tr("User %2 deleted for %1 client")).arg(clientId).arg(userName));
}

void FKRealm::selectUser(const QString& clientId, const QString& userName){
    FKDBIndex clientInd;
    clientInd>>clientBranch>>clientId>>usersNode>>userName;
    _db->writeValue(FKDBValue(userSelected),clientInd,false);
    emit messageRequested(QString(tr("User %2 selected for %1 client")).arg(clientId).arg(userName));
}

void FKRealm::deselectUser(const QString& clientId, const QString& userName){
    FKDBIndex clientInd;
    clientInd>>clientBranch>>clientId>>usersNode>>userName;
    _db->writeValue(FKDBValue(userNotSelected),clientInd,false);
    emit messageRequested(QString(tr("User %2 deselected for %1 client")).arg(clientId).arg(userName));
}

void FKRealm::createNewClientRecord(const QString& clientName, const QString& password){
    FKDBIndex ind;
    ind>>clientBranch>>clientName;
    _db->writeValue(FKDBValue(password),ind,false);
}

QStringList FKRealm::getRoomList(const QVariant& filters) const{
    Q_UNUSED(filters)
    QStringList lst;
    lst<<"Room one"<<"Room next";
    return lst;
}

qint32 FKRealm::getUserStatus(const QString& clientId, const QString& userName){
   FKDBIndex ind;
   ind>>clientBranch>>clientId>>usersNode>>userName;
   return _db->getValue(ind).number();
}

/*!
 * \brief Handle new income guest connection
 */

void FKRealm::incomeConnection(FKConnector* connector){
    FKRealmConnectionManager* mgr=new FKRealmConnectionManager(this,connector,this);
    _guesConnections.insert(mgr);
    emit messageRequested(QString(tr("Got new income connection")));
}

bool FKRealm::isServerConnected(const qint32 id)const{
    return _serverConnections.contains(id);
}

bool FKRealm::isClientConnected(const QString& id)const{
    return _clientConnections.contains(id);
}

bool FKRealm::isClientId(const QString& id)const{
    return FKDBIndex::isNodeName(id);
}

void FKRealm::incomeServer(const qint32 id, FKConnector* connector){
    FKRealmConnectionManagerS* mgr=new FKRealmConnectionManagerS(id,this,connector,this);
    _serverConnections.insert(id,mgr);
    FKBasicEvent submit(FKBasicEventSubject::login,true);
    mgr->sendBasicEvent(&submit);
    emit messageRequested(QString(tr("New server added: "))+QString::number(id));
}

void FKRealm::incomeClient(const QString& id, FKConnector* connector){
    FKRealmConnectionManagerC* mgr=new FKRealmConnectionManagerC(id,this,connector,this);
    _clientConnections.insert(id,mgr);
    FKBasicEvent submit(FKBasicEventSubject::login,true);
    mgr->sendBasicEvent(&submit);
    FKBasicEvent users(FKBasicEventSubject::userList,getUserList(id));
    mgr->sendBasicEvent(&users);
    emit messageRequested(QString(tr("New client added: "))+id);
}

QString FKRealm::getServerPassword(const qint32 id) const{
    FKDBIndex ind;
    ind>>serverBranch>>QString::number(id);
    return _db->getValue(ind,false).string();
}

QString FKRealm::getClientPassword(const QString& id) const{
    FKDBIndex ind;
    ind>>clientBranch>>id;
    return _db->getValue(ind,false).string();
}

QStringList FKRealm::getUserList(const QString& id) const{
    FKDBIndex ind;
    ind>>clientBranch>>id>>usersNode;
    return _db->getProperties(ind);
}

bool FKRealm::isUserExists(const QString& userName) const{
    FKDBIndex ind;
    ind>>userBranch>>userName;
    return _db->hasIndex(ind);
}

bool FKRealm::isUserExists(const QString& userName, const QString& clientId) const{
    FKDBIndex ind;
    ind>>clientBranch>>clientId>>usersNode>>userName;
    return _db->hasIndex(ind);
}

bool FKRealm::isUserName(const QString& name) const{
    return FKDBIndex::isNodeName(name);
}

bool FKRealm::isClientExists(const QString& id) const{
    FKDBIndex ind;
    ind>>clientBranch>>id;
    return _db->hasIndex(ind);
}
