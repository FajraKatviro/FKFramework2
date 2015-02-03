#include "FKAbstractCore.h"

#include <QCoreApplication>
#include <QDir>

#include "FKRealm.h"
#include "FKServerInfrastructure.h"
#include "FKClientInfrastructure.h"
#include "FKFSDB.h"

#include "FKPostOperations.h"
#include "FKOSType.h"
#include "FKLogger.h"

/*!
 * \class FKAbstractCore
 * \brief Class represents common functionality for different application modes.
 */

/*!
 * \brief Constructs empty core object
 */

FKAbstractCore::FKAbstractCore(QObject* parent):QObject(parent),
        _realm(0),_server(0),_infr(0),_customServerId(-1){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Deletes core object
 */

FKAbstractCore::~FKAbstractCore(){
    FK_DBEGIN
    FK_DEND
}

/*!
 * \brief Try identify client infrastructure at realm side
 */

void FKAbstractCore::ausviseClientInfrastructure(const QString id, const QString password){
    if(!clientInfrastructure()){
        emit messageRequested(QString(tr("Unable login: no client infrastructure")));
        return;
    }
    clientInfrastructure()->requestLoginRealm(id,password);
}

/*!
 * \brief Try identify server infrastructure at realm side
 */

void FKAbstractCore::ausviseServerInfrastructure(const qint32 id, const QString password){
    if(!server()){
        emit messageRequested(QString(tr("Unable login: no server infrastructure")));
        return;
    }
    server()->requestLoginRealm(id,password);
}

/*!
 * \fn bool FKAbstractCore::startRealm(int port)
 * \brief Creates realm object and starts it. If port is zero, local (not network) realm is useing
 */

/*!
 * \fn bool FKAbstractCore::startServer(int port,int realmPort,QString realmIP)
 * \brief Creates server object, connect to server and starts it. If port is zero, local server is used; if realmPort is zero, local realm is used. If realm does not exist or not avaliable, returns false
 */

/*!
 * \fn bool FKAbstractCore::startClientInfrastructure(int realmPort,QString realmIP)
 * \brief Creates clientManager object and connects it to realm. If realmPort is zero, local realm is used. If realm does not exists or not avaliable, returns false
 */

/*!
 * \brief Validates user at realm side and creates user record. If clientManager does not exists, returns false
 */

bool FKAbstractCore::createUserRecord(const QString& name){
    if(clientInfrastructure()){
        return clientInfrastructure()->requestUserCreation(name);
    }else{
        emit messageRequested(QString(tr("Unable create user record: no client infrastructure")));
        return false;
    }
}

/*!
 * \brief Validates user at realm side and deletes user record. If clientManager does not exists, returns false
 */

bool FKAbstractCore::deleteUserRecord(const QString& name){
    if(clientInfrastructure()){
        return clientInfrastructure()->requestUserDeletion(name);
    }else{
        emit messageRequested(QString(tr("Unable delete user record: no client infrastructure")));
        return false;
    }
}

/*!
 * \fn bool FKAbstractCore::startUser(const QString& name)
 * \brief Validates user at realm side and creates user object. If clientInfrastructure does not exists, returns false
 */

bool FKAbstractCore::startUser(const QString &name){
    if(clientInfrastructure()){
        clientInfrastructure()->requestUserAuthorization(name);
        return true;
    }else{
        emit messageRequested(QString(tr("Unable start user: no client infrastructure")));
        return false;
    }
}

/*!
 * \brief Try stop selected user. Returns false if client infrastructure does not exists
 */

bool FKAbstractCore::stopUser(const QString& name){
    if(clientInfrastructure()){
        clientInfrastructure()->requestUserDeauthorization(name);
        return true;
    }else{
        emit messageRequested(QString(tr("Unable stop user: no client infrastructure")));
        return false;
    }
}

/*!
 * \fn bool FKAbstractCore::createRoom(const QMap<QString,QVariant>& roomData)
 * \brief Request at realm side room creation and join it. Creation occures at server side
 */

/*!
 * \fn bool FKAbstractCore::joinRoom(const qint32& id)
 * \brief Request at realm side to join specified room created at some server. On success, interface, world and room are creating at client side, all clients are connecting to server and then object syncronization occures.
 */

/*!
 * \fn bool FKAbstractCore::stopRealm()
 * \brief Tries to stop realm and delete realm object. All connected servers and clients would be disconnected and destroyed
 */

/*!
 * \fn bool FKAbstractCore::stopServer()
 * \brief Tries to stop server and delete server object. All connected realms and clients would be disconnected
 */

/*!
 * \fn bool FKAbstractCore::stopClientInfrastructure()
 * \brief Tries to stop client manager and delete it. All managed clients would be disconnected from server and destroyed, manager would be disconnected from realm
 */

/*!
 * \fn bool FKAbstractCore::quitRoom()
 * \brief Tries to quit current room. This is client side function and requests clientManager quit active room. On success, clients would be disconnected; room, room-world and room-interface would be destroyed
 */

/*!
 * \brief Quit the application by call qApp->quit()
 */

void FKAbstractCore::quitApplication(){
    qApp->quit();
}

/*!
 * \fn void FKAbstractCore::setApplicationFilesList()
 * \brief Build new list of application files identifiers
 */

/*!
 * \fn const FKFilesList& FKAbstractCore::applicationFilesList()const
 * \brief Return previously setted list of application files identifiers
 */

/*!
 * \brief Add command to post-execution script
 */

void FKAbstractCore::addPostExecutionCommand(const QString& command){
//    std::cout<<'@'<<QString::number(command.size()).rightJustified(4,'0').toStdString()<<command.toStdString();
}


/*!
 * \brief Constructs application files list object for application versionalizing
 */

FKFilesList FKAbstractCore::buildApplicationFilesList()const{
    FKFilesList lst;
    QDir dir(qApp->applicationDirPath());
    dir.cdUp();
//    lst.add(QFileInfo(dir.canonicalPath()),FK_OS_COMMON);
    QMap<qint8,QString> platforms;
    platforms.insert(FKOSType::nix,FKOSDir::nix);
    platforms.insert(FKOSType::ard,FKOSDir::ard);
    platforms.insert(FKOSType::mac,FKOSDir::mac);
    platforms.insert(FKOSType::ios,FKOSDir::ios);
    platforms.insert(FKOSType::win,FKOSDir::win);
    platforms.insert(FKOSType::common,QStringLiteral("qml"));
    for(auto i=platforms.constBegin();i!=platforms.constEnd();++i){
        if(dir.cd(i.value())){
            lst.addRecursive(QFileInfo(dir.canonicalPath()),i.key());
            dir.cdUp();
        }
    }
    return lst;
}

/*!
 * \brief Assign started by this core realm infrastructure
 */

void FKAbstractCore::setRealm(FKRealm* realm){
    _realm=realm;
    connect(_realm,SIGNAL(messageRequested(QString)),SIGNAL(systemMessageRequested(QString)));
    FKDataBase* db=new FKFSDB(_realm);
    db->setPath(realmDatabasePath());
    _realm->setDataBase(db);
}

/*!
 * \brief Assign started by this core server infrastructure
 */

void FKAbstractCore::setServer(FKServerInfrastructure* server){
    _server=server;
    connect(server,SIGNAL(waitingForAnswerChanged(FKInfrastructureType)),SLOT(waitingForAnswerChanged(FKInfrastructureType)));
    connect(server,SIGNAL(connectedToRealm()),SLOT(serverConnectedToRealmSlot()));
    connect(server,SIGNAL(disconnectedFromRealm()),SIGNAL(serverDisconnectedFromRealmSlot()));
    connect(server,SIGNAL(loggedIn()),SLOT(serverLoggedInSlot()));
    connect(server,SIGNAL(messageRequested(QString)),SIGNAL(messageRequested(QString)));
    FKDataBase* db=new FKFSDB(_server);
    db->setPath(serverDatabasePath());
    _server->setDataBase(db);
}

/*!
 * \brief Assign started by this core client infrastructure
 */

void FKAbstractCore::setClientInfrastructure(FKClientInfrastructure* infr){
    _infr=infr;
    connect(infr,SIGNAL(waitingForAnswerChanged(FKInfrastructureType)),SLOT(waitingForAnswerChanged(FKInfrastructureType)));
    connect(infr,SIGNAL(connectedToRealm()),SIGNAL(clientConnectedToRealm()));
    connect(infr,SIGNAL(disconnectedFromRealm()),SIGNAL(clientDisconnectedFromRealm()));
    connect(infr,SIGNAL(loggedIn()),SIGNAL(clientLoggedIn()));
    connect(infr,SIGNAL(messageRequested(QString)),SIGNAL(messageRequested(QString)));
    connect(infr,SIGNAL(userPoolChanged()),SIGNAL(userPoolChanged()));
    connect(infr,SIGNAL(activeUsersChanged()),SIGNAL(userListChanged()));
    connect(infr,SIGNAL(customServerRequested(qint32,QString)),SLOT(createCustomServer(qint32,QString)));
}

/*!
 * \brief Returns started realm or 0
 */

FKRealm* FKAbstractCore::realm(){
    return _realm;
}

/*!
 * \brief Returns started server or 0
 */

FKServerInfrastructure* FKAbstractCore::server(){
    return _server;
}

/*!
 * \brief Returns started client infrastructure or 0
 */

FKClientInfrastructure* FKAbstractCore::clientInfrastructure(){
    return _infr;
}

/*!
 * \brief Returns path for realm database. Default value is data/realm subfolder relative to application general folder
 */

QString FKAbstractCore::realmDatabasePath() const{
    QDir dir(qApp->applicationDirPath());
    changeDir(dir,QString("data"));
    changeDir(dir,QString("realm"));
    return dir.canonicalPath();
}

/*!
 * \brief Returns path for server database. Default value is data/server subfolder relative to application general folder
 */

QString FKAbstractCore::serverDatabasePath() const{
    QDir dir(qApp->applicationDirPath());
    changeDir(dir,QString("data"));
    changeDir(dir,QString("server"));
    return dir.canonicalPath();
}

void FKAbstractCore::waitingForAnswerChanged(FKInfrastructureType t){
    if(FKInfrastructureType::Realm==t){
        emit waitingForRealmAnswerChanged();
    }else if(FKInfrastructureType::Server==t){
        emit waitingForServerAnswerChanged();
    }
}

void FKAbstractCore::createCustomServer(const qint32 id, const QString password){
    QString error;
    if(server()){
        error=QString(tr("Unable create custom server: server already exists"));
    }else if(_customServerId>0 || !_customServerPassword.isEmpty()){
        error=QString(tr("Unable create custom server: current server data is not empty"));
    }

    if(error.isEmpty()){
        _customServerId=id;
        _customServerPassword=password;
        startServer(customServerPort(),clientInfrastructure()->realmPort(),clientInfrastructure()->realmIP());
    }else{
        emit messageRequested(error);
    }
}

void FKAbstractCore::serverConnectedToRealmSlot(){
    emit serverConnectedToRealm();
    if(_customServerId>0){
        ausviseServerInfrastructure(_customServerId,_customServerPassword);
    }
}

void FKAbstractCore::serverDisconnectedFromRealmSlot(){
    emit serverDisconnectedFromRealm();
    _customServerId=-1;
    _customServerPassword.clear();
}

void FKAbstractCore::serverLoggedInSlot(){
    emit serverLoggedIn();
}

QDir& FKAbstractCore::changeDir(QDir& dir, const QString& name){
    if(!dir.cd(name)){
        if(!dir.mkdir(name)){
            FK_MLOGV("Path can not be created",dir.canonicalPath()+QString("/")+name)
        }
        dir.cd(name);
    }
    return dir;
}

/*!
 * \brief Restart application and run post-execution script
 */

void FKAbstractCore::restartApplication(){
    addPostExecutionCommand(FKPostOperations::restartApplication);
    qApp->quit();
}

/*!
 * \brief Returns true if core is waiting for realm's answer
 */

bool FKAbstractCore::waitingForRealmAnswer() const{
    return _infr && _infr->waitingForAnswer(FKInfrastructureType::Realm);
}

/*!
 * \brief Returns true if core is waiting for server's answer
 */

bool FKAbstractCore::waitingForServerAnswer() const{
    return _infr && _infr->waitingForAnswer(FKInfrastructureType::Server);
}

/*!
 * \brief Returns list of currently avaliable not selected users for current client infrastructure
 */

QStringList FKAbstractCore::getUserPool() const{
    return _infr ? _infr->userPool() : QStringList();
}

/*!
 * \brief Returns list of currently selected users for current client infrastructure
 */

QStringList FKAbstractCore::getUserList() const{
    return _infr ? _infr->activeUsers() : QStringList();
}

/*!
 * \brief Returns full list of registered users on current realm
 */

QStringList FKAbstractCore::realmUserList()const{
    return _realm ? _realm->userList() : QStringList();
}

/*!
 * \brief Try create new client record for started realm
 */

void FKAbstractCore::createClientRecord(const QString clientName, const QString password){
    if(_realm){
        _realm->createClientRecord(clientName,password);
    }else{
        emit messageRequested(QString(tr("Unable create client record: no realm infrastructure")));
    }
}

/*!
 * \brief Try create new server record for started realm
 */

void FKAbstractCore::createServerRecord(const QString password){
    if(_realm){
        _realm->createServerRecord(password);
    }else{
        emit messageRequested(QString(tr("Unable create server record: no realm infrastructure")));
    }
}

void FKAbstractCore::registerRoomType(const QString roomType){
    if(_realm){
        _realm->registerRoomType(roomType);
    }else{
        emit messageRequested(QString(tr("Unable register room type: no realm infrastructure")));
    }
}

void FKAbstractCore::registerServerRoomType(const QString roomType){
    if(_server){
        _server->registerRoomTypeRequest(roomType);
    }else{
        emit messageRequested(QString(tr("Unable register room type: no server infrastructure")));
    }
}

/*!
 * \brief Try create request to realm to create custom room at self-side
 */

void FKAbstractCore::createRoomRequest(const QString roomName, const QString roomType){
    if(!clientInfrastructure()){
        emit messageRequested(QString(tr("Unable create room: no client infrastructure")));
    }else{
        clientInfrastructure()->createRoomRequest(roomName,roomType);
    }
}

/*!
 * \brief Try create room for any avaliable server
 */

void FKAbstractCore::createRoomRequestRealm(const QString roomName, const QString roomType){
    if(!realm()){
        emit messageRequested(QString(tr("Unable create room: no realm started")));
    }else{
        realm()->createRoomRealmRequest(roomName,roomType);
    }
}

void FKAbstractCore::createCustomServerRequest(){
    if(!clientInfrastructure()){
        emit messageRequested(QString(tr("Unable start custom server: no client infrastructure")));
    }else{
        clientInfrastructure()->requestCustomServer();
    }
}

/*!
 * \fn void FKAbstractCore::messageRequested(const QString msg)
 * \brief This signal emitted when core want to show message to user
 */

/*!
 * \fn void FKAbstractCore::clientConnectedToRealm()
 * \brief Signal emitted when client infrastructure successfully connects as guest to realm
 */

/*!
 * \fn void FKAbstractCore::clientDisconnectedFromRealm()
 * \brief Signal emitted when client infrastructure disconnects from realm
 */

/*!
 * \fn void FKAbstractCore::clientLoggedIn()
 * \brief Signal emitted when guest client infrastructure from this core recieve ausvice submit from realm
 */

/*!
 * \fn void FKAbstractCore::waitingForRealmAnswerChanged()
 * \brief This signal emitted when core create request to realm or get an answer
 */

/*!
 * \fn void FKAbstractCore::waitingForServerAnswerChanged()
 * \brief This signal emitted when core create request to server or get an answer
 */

/*!
 * \fn void FKAbstractCore::userPoolChanged()
 * This signal emitted when client infrastructure changes it's user pool
 */

/*!
 * \fn void FKAbstractCore::userListChanged()
 * This signal emitted when client infrastructure changes it's currently selected users list
 */
