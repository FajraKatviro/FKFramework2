#include "FKSimpleCore.h"

#include <QCoreApplication>
#include <QDir>

#include "FKRealm.h"
#include "FKServerInfrastructure.h"
#include "FKClientInfrastructure.h"
#include "FKFSDB.h"
#include "FKVersionList.h"
#include "FKInstructionObject.h"
#include "FKRoomInviteData.h"
#include "FKPathResolver.h"
#include "FKFactory.h"
#include "FKLocalConnector.h"

#include "FKLogger.h"

/*!
\class FKSimpleCore
\brief Component provides basic infrastructures common management
*/

/*!
\class FKRealmComponent
\brief Provides threaded FKRealm access
*/

/*!
\class FKServerComponent
\brief Provides threaded FKServerInfrastructure accesss
*/

/*!
\class FKClientComponent
\brief Provides threaded FKClientInfrastructure accesss
*/

FKRealmComponent::FKRealmComponent(QObject* parent):FKThreadedComponent(parent){
    FK_CBEGIN
    FK_CEND
}

FKRealmComponent::~FKRealmComponent(){
    FK_DBEGIN
    FK_DEND
}

void FKRealmComponent::startComponent(){
    if(!component()){
        FKRealm* realm=static_cast<FKRealm*>(componentFactory()->newInstance());
        connect(realm,SIGNAL(messageRequested(QString)),this,SIGNAL(messageRequested(QString)));
        QString dbPath(FKPathResolver::realmDatabasePath());
        QDir(dbPath).mkpath(".");
        FKDataBase* db=new FKFSDB(realm);
        db->setPath(dbPath);
        realm->setDataBase(db);
        FKThreadedComponent::startComponent(realm);
        emit messageRequested(QString(tr("Started")));
        emit started();
    }
}

QStringList FKRealmComponent::userList(){
    QStringList lst;
    if(!FK_THREAD_GETTER(QStringList,lst,userList)){
        emit messageRequested(QString(tr("Unable get user list")));
    }
    return lst;
}

QStringList FKRealmComponent::userList(const QString clientId){
    QStringList lst;
    if(!FK_THREAD_GETTER_ARG(QStringList,lst,getUserList,QString,clientId)){
        emit messageRequested(QString(tr("Unable get user list")));
    }
    return lst;
}

QStringList FKRealmComponent::clientList(){
    QStringList lst;
    if(!FK_THREAD_GETTER(QStringList,lst,clientList)){
        emit messageRequested(QString(tr("Unable get client list")));
    }
    return lst;
}

QStringList FKRealmComponent::connectedClientList(){
    QStringList lst;
    if(!FK_THREAD_GETTER(QStringList,lst,connectedClientList)){
        emit messageRequested(QString(tr("Unable get client list")));
    }
    return lst;
}

QStringList FKRealmComponent::activeClientList(){
    QStringList lst;
    if(!FK_THREAD_GETTER(QStringList,lst,connectedClientList)){
        emit messageRequested(QString(tr("Unable get client list")));
    }
    return lst;
}

QStringList FKRealmComponent::serverList(){
    QStringList lst;
    if(!FK_THREAD_GETTER(QStringList,lst,serverList)){
        emit messageRequested(QString(tr("Unable get server list")));
    }
    return lst;
}

QStringList FKRealmComponent::serverList(const QString roomType){
    if(roomType.isEmpty())return serverList();
    QStringList lst;
    if(!FK_THREAD_GETTER_ARG(QStringList,lst,getServersForRoomType,QString,roomType)){
        emit messageRequested(QString(tr("Unable get server list for %1 room type")).arg(roomType));
    }
    return lst;
}

QList<int> FKRealmComponent::connectedServerList(){
    QList<qint32> lst;
    if(!FK_THREAD_GETTER(QList<qint32>,lst,connectedServerList)){
        emit messageRequested(QString(tr("Unable get connected server list")));
    }
    return lst;
}

QList<int> FKRealmComponent::connectedServerList(const QString roomType){
    if(roomType.isEmpty())return connectedServerList();
    QList<qint32> lst;
    if(!FK_THREAD_GETTER(QList<qint32>,lst,getConnectedServersForRoomType)){
        emit messageRequested(QString(tr("Unable get connected server list for %1 room type")).arg(roomType));
    }
    return lst;
}

QList<int> FKRealmComponent::avaliableServerList(){
    QList<qint32> lst;
    if(!FK_THREAD_GETTER(QList<qint32>,lst,getAvaliableServers)){
        emit messageRequested(QString(tr("Unable get avaliable server list")));
    }
    return lst;
}

QList<int> FKRealmComponent::avaliableServerList(const QString roomType){
    if(roomType.isEmpty())return avaliableServerList();
    QList<qint32> lst;
    if(!FK_THREAD_GETTER(QList<qint32>,lst,getAvaliableServersForRoomType)){
        emit messageRequested(QString(tr("Unable get avaliable server list for %1 room type")).arg(roomType));
    }
    return lst;
}

QStringList FKRealmComponent::roomTypeList(){
    QStringList lst;
    if(!FK_THREAD_GETTER(QStringList,lst,getRegisteredRoomTypes)){
        emit messageRequested(QString(tr("Unable asquire room type list")));
    }
    return lst;
}

QStringList FKRealmComponent::roomTypeList(const qint32 serverId){
    if(serverId<0)return roomTypeList();
    QStringList lst;
    if(!FK_THREAD_GETTER_ARG(QStringList,lst,serverAvaliableRoomTypes,qint32,serverId)){
        emit messageRequested(QString(tr("Unable asquire room type list for %1 server")).arg(QString::number(serverId)));
    }
    return lst;
}

QStringList FKRealmComponent::activeRoomList(){
    return activeRoomList(QVariant());
}

QStringList FKRealmComponent::activeRoomList(const QVariant filter){
    QStringList lst;
    if(!FK_THREAD_GETTER_ARG(QStringList,lst,getRoomList,QVariant,filter)){
        emit messageRequested(QString(tr("Unable get room list")));
    }
    return lst;
}

void FKRealmComponent::createClientRecord(const QString clientName, const QString password){
    if(!callMethod("createClientRecord",clientName,"QString",password,"QString")){
        emit messageRequested(QString(tr("Unable create client record")));
    }
}

void FKRealmComponent::deleteClientRecord(const QString clientName){
    if(!FK_THREAD_CALL_ARG(deleteClientRecord,QString,clientName)){
        emit messageRequested(QString(tr("Unable delete client record")));
    }
}

void FKRealmComponent::createServerRecord(const QString password){
    if(!FK_THREAD_CALL_ARG(createServerRecord,QString,password)){
        emit messageRequested(QString(tr("Unable create server record")));
    }
}

void FKRealmComponent::deleteServerRecord(const qint32 serverId){
    if(!FK_THREAD_CALL_ARG(deleteServerRecord,qint32,serverId)){
        emit messageRequested(QString(tr("Unable remove server record")));
    }
}

void FKRealmComponent::dropServer(const qint32 serverId){
    if(!FK_THREAD_CALL_ARG(dropServer,qint32,serverId)){
        emit messageRequested(QString(tr("Unable drop server")));
    }
}

void FKRealmComponent::registerRoomType(const QString roomType){
    if(!FK_THREAD_CALL_ARG(registerRoomType,QString,roomType)){
        emit messageRequested(QString(tr("Unable register room type")));
    }
}

void FKRealmComponent::removeRoomType(const QString roomType){
    if(!FK_THREAD_CALL_ARG(removeRoomType,QString,roomType)){
        emit messageRequested(QString(tr("Unable remove room type")));
    }
}

void FKRealmComponent::createRoomRequest(const QString roomName, const QString roomType){
    if(!callMethod("createRoomRequest",roomName,"QString",roomType,"QString")){
        emit messageRequested(QString(tr("Unable request create room")));
    }
}

void FKRealmComponent::setPort(const qint32 port){
    if(!FK_THREAD_CALL_ARG(setPort,qint32,port)){
        emit messageRequested(QString(tr("Unable set port")));
    }
}

void FKRealmComponent::guestConnection(FKConnector* connector){
    connector->moveToThread(component()->thread());
    const bool activation=true;
    if(!callMethod("incomeConnection",connector,"FKConnector*",activation,"bool")){
        emit messageRequested(QString(tr("Unable add guest connection")));
    }
}

void FKRealmComponent::componentThreadQuit(){
    FKThreadedComponent::componentThreadQuit();
    emit messageRequested(QString("Stopped"));
}

FKServerComponent::FKServerComponent(QObject* parent):FKThreadedComponent(parent){
    FK_CBEGIN
    FK_CEND
}

FKServerComponent::~FKServerComponent(){
    FK_DBEGIN
    FK_DEND
}

void FKServerComponent::startComponent(){
    if(!component()){
        FKServerInfrastructure* server=static_cast<FKServerInfrastructure*>(componentFactory()->newInstance());
        connect(server,SIGNAL(waitingForAnswerChanged(FKInfrastructureType)),SIGNAL(waitingForRealmAnswerChanged()));
        connect(server,SIGNAL(connectedToRealm()),SIGNAL(connectedToRealm()));
        connect(server,SIGNAL(disconnectedFromRealm()),SIGNAL(disconnectedFromRealm()));
        connect(server,SIGNAL(loggedIn()),SIGNAL(loggedIn()));
        connect(server,SIGNAL(messageRequested(QString)),SIGNAL(messageRequested(QString)));
        connect(server,SIGNAL(roomInstruction(FKInstructionObject)),SIGNAL(roomInstruction(FKInstructionObject)));
        QString dbPath(FKPathResolver::serverDatabasePath());
        QDir(dbPath).mkpath(".");
        FKDataBase* db=new FKFSDB(server);
        db->setPath(dbPath);
        server->setDataBase(db);
        FKThreadedComponent::startComponent(server);
        emit messageRequested(QString(tr("Started")));
        emit started();
    }
}

QStringList FKServerComponent::roomTypeList(){
    QStringList lst;
    if(!FK_THREAD_GETTER(QStringList,lst,registeredRoomTypes)){
        emit messageRequested(QString(tr("Unable asquire room type list")));
    }
    return lst;
}

bool FKServerComponent::isLogged(){
    bool result;
    if(!FK_THREAD_GETTER(bool,result,isLogged)){
        result = false;
        emit messageRequested(QString(tr("Unable asquire logged status from server")));
    }
    return result;
}

bool FKServerComponent::isConnectedToRealm(){
    bool result;
    if(!FK_THREAD_GETTER(bool,result,isConnectedToRealm)){
        result = false;
        emit messageRequested(QString(tr("Unable asquire connected to realm status from server")));
    }
    return result;
}

void FKServerComponent::ausvise(const qint32 id, const QString password){
    if(!callMethod("requestLoginRealm",id,"qint32",password,"QString")){
        emit messageRequested(QString(tr("Unable request log in")));
    }
}

void FKServerComponent::registerRoomType(const QString roomType){
    if(!FK_THREAD_CALL_ARG(registerRoomTypeRequest,QString,roomType)){
        emit messageRequested(QString(tr("Unable register room type")));
    }
}

void FKServerComponent::removeRoomType(const QString roomType){
    if(!FK_THREAD_CALL_ARG(removeRoomTypeRequest,QString,roomType)){
        emit messageRequested(QString(tr("Unable remove room type")));
    }
}

void FKServerComponent::setPort(const qint32 port){
    if(!FK_THREAD_CALL_ARG(setPort,qint32,port)){
        emit messageRequested(QString(tr("Unable set server port")));
    }
}

void FKServerComponent::setRealmConnectionSettings(const QString realmIP, const qint32 realmPort){
    if(!callMethod("setRealmConnectionSettings",realmIP,"QString",realmPort,"qint32")){
        emit messageRequested(QString(tr("Unable set server realm connection settings")));
    }
}

void FKServerComponent::realmConnection(FKConnector* connector){
    connector->moveToThread(component()->thread());
    if(!FK_THREAD_CALL_ARG(realmConnection,FKConnector*,connector)){
        emit messageRequested(QString(tr("Unable set realm connection for server")));
    }
}

void FKServerComponent::guestConnection(FKConnector* connector){
    connector->moveToThread(component()->thread());
    if(!FK_THREAD_CALL_ARG(clientConnection,FKConnector*,connector)){
        emit messageRequested(QString(tr("Unable add server guest connection")));
    }
}

void FKServerComponent::handleRoomInstruction(FKInstructionObject instruction){
    if(!isRunning())return;
    if(!FK_THREAD_CALL_ARG(handleRoomInstruction,FKInstructionObject,instruction)){
        emit messageRequested(QString(tr("Unable handle room engine instruction")));
    }
}

void FKServerComponent::componentThreadQuit(){
    FKThreadedComponent::componentThreadQuit();
    emit messageRequested(QString("Stopped"));
}

bool FKServerComponent::waitingForRealmAnswer(){
    bool result;
    if(!FK_THREAD_GETTER(bool,result,waitingForAnswer)){
        result=false;
    }
    return result;
}

FKClientComponent::FKClientComponent(QObject* parent):FKThreadedComponent(parent){
    FK_CBEGIN
    FK_CEND
}

FKClientComponent::~FKClientComponent(){
    FK_DBEGIN
    FK_DEND
}

void FKClientComponent::startComponent(){
    if(!component()){
        FKClientInfrastructure* client=static_cast<FKClientInfrastructure*>(componentFactory()->newInstance());
        connect(client,SIGNAL(waitingForRealmAnswerChanged()),SIGNAL(waitingForRealmAnswerChanged()));
        connect(client,SIGNAL(waitingForServerAnswerChanged()),SIGNAL(waitingForServerAnswerChanged()));
        connect(client,SIGNAL(connectedToRealm()),SIGNAL(clientConnectedToRealm()));
        connect(client,SIGNAL(disconnectedFromRealm()),SIGNAL(clientDisconnectedFromRealm()));
        connect(client,SIGNAL(loggedIn()),SIGNAL(clientLoggedIn()));
        connect(client,SIGNAL(connectedToServer()),SIGNAL(clientConnectedToServer()));
        connect(client,SIGNAL(disconnectedFromServer()),SIGNAL(clientDisconnectedFromServer()));
        connect(client,SIGNAL(loggedInServer()),SIGNAL(clientLoggedInServer()));
        connect(client,SIGNAL(messageRequested(QString)),SIGNAL(messageRequested(QString)));
        connect(client,SIGNAL(roomInstruction(FKInstructionObject)),SIGNAL(roomInstruction(FKInstructionObject)));
        QString dbPath(FKPathResolver::clientDatabasePath());
        QDir(dbPath).mkpath(".");
        FKDataBase* db=new FKFSDB(client);
        db->setPath(dbPath);
        client->setDataBase(db);
        FKThreadedComponent::startComponent(client);
        emit messageRequested(QString(tr("Started")));
        emit started();
    }
}

void FKClientComponent::setRealmConnectionSettings(const QString realmIP, const qint32 realmPort){
    if(!callMethod("setRealmConnectionSettings",realmIP,"QString",realmPort,"qint32")){
        emit messageRequested(QString(tr("Unable set server realm connection settings")));
    }
}

void FKClientComponent::realmConnection(FKConnector* connector){
    connector->moveToThread(component()->thread());
    if(!FK_THREAD_CALL_ARG(realmConnection,FKConnector*,connector)){
        emit messageRequested(QString(tr("Unable set realm connection for client")));
    }
}

void FKClientComponent::ausvise(const QString id, const QString password){
    if(!callMethod("requestLoginRealm",id,"QString",password,"QString")){
        emit messageRequested(QString(tr("Unable request log in")));
    }
}

void FKClientComponent::handleRoomInstruction(FKInstructionObject instruction){
    if(!isRunning())return;
    if(!FK_THREAD_CALL_ARG(handleRoomInstruction,FKInstructionObject,instruction)){
        emit messageRequested(QString(tr("Unable handle room engine instruction")));
    }
}

bool FKClientComponent::waitingForRealmAnswer() const{
    bool result;
    if(!FK_THREAD_GETTER(bool,result,waitingForRealmAnswer)){
        result=false;
    }
    return result;
}

bool FKClientComponent::waitingForServerAnswer() const{
    bool result;
    if(!FK_THREAD_GETTER(bool,result,waitingForServerAnswer)){
        result=false;
    }
    return result;
}

FKSimpleCore::FKSimpleCore(QObject* parent):QObject(parent){
    FK_CBEGIN        
    qRegisterMetaTypeStreamOperators<FKVersionList>("FKVersionList");
    qRegisterMetaTypeStreamOperators<FKInstructionObject>("FKInstructionObject");
    qRegisterMetaType<FKRoomInviteData>();
    qRegisterMetaType<FKInfrastructureType>();
    qRegisterMetaType<FKConnector*>();
    qRegisterMetaType<FKConnector::Status>();
    qRegisterMetaType<FKInstructionObject>();
    //qmlRegisterType<FKRealmComponent*>();
    //qRegisterMetaType<FKServerComponent*>();
    //qRegisterMetaType<FKClientComponent*>();
    FK_CEND
}

/*!
 * \brief Deletes core object
 */

FKSimpleCore::~FKSimpleCore(){
    FK_DBEGIN
    FK_DEND
}

bool FKSimpleCore::startRealmInfrastructure(const qint32 port){
    if(!_realmComponent->isRunning()){
        _realmComponent->startComponent();
        _realmComponent->setPort(port);
        return true;
    }else{
        emit messageRequested(QString(tr("Unable start realm: already started")));
        return false;
    }
}

bool FKSimpleCore::startServerInfrastructure(const qint32 port, const qint32 realmPort, const QString& realmIP){
    if(_realmComponent->isRunning()){
        if(!_serverComponent->isRunning()){
            _serverComponent->startComponent();
            _serverComponent->setPort(port);
            _serverComponent->setRealmConnectionSettings(realmIP,realmPort);

            FKLocalConnector* serverSideConnector=new FKLocalConnector;
            FKLocalConnector* realmSideConnector=new FKLocalConnector;
            realmSideConnector->join(serverSideConnector);
            _serverComponent->realmConnection(serverSideConnector); //this must be first
            _realmComponent->guestConnection(realmSideConnector); //this must be seconds
            return true;
        }else{
            emit messageRequested(QString(tr("Unable start server: already started")));
        }
    }else{
        emit messageRequested(QString(tr("Unable start local server: no realm started")));
    }
    return false;
}

bool FKSimpleCore::startClientInfrastructure(const int realmPort, const QString& realmIP){
    if(_realmComponent->isRunning()){
        if(!_clientComponent->isRunning()){
            _clientComponent->startComponent();
            _clientComponent->setRealmConnectionSettings(realmIP,realmPort);

            FKLocalConnector* clientSideConnector=new FKLocalConnector;
            FKLocalConnector* realmSideConnector=new FKLocalConnector;
            realmSideConnector->join(clientSideConnector);
            _clientComponent->realmConnection(clientSideConnector); //this must be first
            _realmComponent->guestConnection(realmSideConnector); //this must be seconds
            return true;
        }else{
            emit messageRequested(QString(tr("Unable start client: already started")));
        }
    }else{
        emit messageRequested(QString(tr("Unable start local app client: no realm started")));
    }
    return false;
}

bool FKSimpleCore::stopRealmInfrastructure(){
    if(_realmComponent->isRunning()){
        _realmComponent->stopComponent();
        return true;
    }else{
        emit messageRequested(QString(tr("Unable stop realm: not started")));
    }
    return false;
}

bool FKSimpleCore::stopServerInfrastructure(){
    if(_serverComponent->isRunning()){
        _serverComponent->stopComponent();
        return true;
    }else{
        emit messageRequested(QString(tr("Unable stop server: not started")));
    }
    return false;
}

bool FKSimpleCore::stopClientInfrastructure(){
    if(_clientComponent->isRunning()){
        _clientComponent->stopComponent();
        return true;
    }else{
        emit messageRequested(QString(tr("Unable stop client: not started")));
    }
    return false;
}

void FKSimpleCore::quitApplication(){
    qApp->quit();
}

void FKSimpleCore::initComponents(){
    createComponents();
    installComponents();
    installComponentFactories();
    emit realmComponentChanged();
    emit serverComponentChanged();
    emit clientComponentChanged();
}

void FKSimpleCore::createComponents(){
    _realmComponent=new FKRealmComponent(this);
    _serverComponent=new FKServerComponent(this);
    _clientComponent=new FKClientComponent(this);
}

void FKSimpleCore::installComponents(){
    connect(_realmComponent,SIGNAL(messageRequested(QString)),SLOT(realmMessage(QString)));
    connect(_serverComponent,SIGNAL(messageRequested(QString)),SLOT(serverMessage(QString)));
    connect(_clientComponent,SIGNAL(messageRequested(QString)),SLOT(clientMessage(QString)));
}

void FKSimpleCore::installComponentFactories(){
    _realmComponent->setComponentFactory(new FKFactoryObjectCreator<FKRealm>());
    _serverComponent->setComponentFactory(new FKFactoryObjectCreator<FKServerInfrastructure>());
    _clientComponent->setComponentFactory(new FKFactoryObjectCreator<FKClientInfrastructure>());
}

void FKSimpleCore::realmMessage(QString msg){
    msg.prepend(QLatin1String("Realm->"));
    emit messageRequested(msg);
}

void FKSimpleCore::serverMessage(QString msg){
    msg.prepend(QLatin1String("Server->"));
    emit messageRequested(msg);
}

void FKSimpleCore::clientMessage(QString msg){
    msg.prepend(QLatin1String("Client->"));
    emit messageRequested(msg);
}
