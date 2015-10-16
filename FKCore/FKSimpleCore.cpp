#include "FKSimpleCore.h"

#include <QCoreApplication>
#include <QDir>

#include "FKRealm.h"
#include "FKServerInfrastructure.h"
#include "FKClientInfrastructure.h"
#include "FKFSDB.h"
#include "FKVersionList.h"
#include "FKRoomInviteData.h"
#include "FKPathResolver.h"
#include "FKFactory.h"

#include "FKLogger.h"

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
        emit messageRequested(QString(tr("Realm started")));
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

QList<qint32> FKRealmComponent::connectedServerList(){
    QList<qint32> lst;
    if(!FK_THREAD_GETTER(QList<qint32>,lst,connectedServerList)){
        emit messageRequested(QString(tr("Unable get server list")));
    }
    return lst;
}

QStringList FKRealmComponent::roomTypeList(){
    QStringList lst;
    if(!FK_THREAD_GETTER(QStringList,lst,getRegisteredRoomTypes)){
        emit messageRequested(QString(tr("Unable asquire room type list from realm")));
    }
    return lst;
}

QStringList FKRealmComponent::roomTypeList(const qint32 serverId){
    if(serverId<0)return roomTypeList();
    QStringList lst;
    if(!FK_THREAD_GETTER_ARG(QStringList,lst,serverAvaliableRoomTypes,qint32,serverId)){
        emit messageRequested(QString(tr("Unable asquire room type list from realm for %1 server")).arg(QString::number(serverId)));
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
        emit messageRequested(QString(tr("Unable create client record for realm")));
    }
}

void FKRealmComponent::deleteClientRecord(const QString clientName){
    if(!FK_THREAD_CALL_ARG(deleteClientRecord,QString,clientName)){
        emit messageRequested(QString(tr("Unable delete client record for realm")));
    }
}

void FKRealmComponent::createServerRecord(const QString password){
    if(!FK_THREAD_CALL_ARG(createServerRecord,QString,password)){
        emit messageRequested(QString(tr("Unable create server record for realm")));
    }
}

void FKRealmComponent::deleteServerRecord(const qint32 serverId){
    if(!FK_THREAD_CALL_ARG(deleteServerRecord,qint32,serverId)){
        emit messageRequested(QString(tr("Unable remove server record from realm")));
    }
}

void FKRealmComponent::registerRoomType(const QString roomType){
    if(!FK_THREAD_CALL_ARG(registerRoomType,QString,roomType)){
        emit messageRequested(QString(tr("Unable register room type for realm")));
    }
}

void FKRealmComponent::removeRoomType(const QString roomType){
    if(!FK_THREAD_CALL_ARG(removeRoomType,QString,roomType)){
        emit messageRequested(QString(tr("Unable remove room type from realm")));
    }
}

void FKRealmComponent::setPort(const qint32 port){
    if(!FK_THREAD_CALL_ARG(setPort,qint32,port)){
        emit messageRequested(QString(tr("Unable set realm port")));
    }
}

void FKRealmComponent::guestConnection(FKConnector* connector){
    if(!FK_THREAD_CALL_ARG(incomeConnection,FKConnector*,connector)){
        emit messageRequested(QString(tr("Unable add realm guest connection")));
    }
}

void FKRealmComponent::componentThreadQuit(){
    FKThreadedComponent::componentThreadQuit();
    emit messageRequested(QString("Realm stopped"));
}

FKServerComponent::FKServerComponent(QObject* parent):FKThreadedComponent(parent){
    FK_CBEGIN
    FK_CEND
}

FKServerComponent::~FKServerComponent(){
    FK_DBEGIN
    FK_DEND
}

FKClientComponent::FKClientComponent(QObject* parent):FKThreadedComponent(parent){
    FK_CBEGIN
    FK_CEND
}

FKClientComponent::~FKClientComponent(){
    FK_DBEGIN
    FK_DEND
}
FKSimpleCore::FKSimpleCore(QObject* parent):QObject(parent){
    FK_CBEGIN        
    qRegisterMetaTypeStreamOperators<FKVersionList>("FKVersionList");
    qRegisterMetaType<FKRoomInviteData>();
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
    }
    return false;
}

bool FKSimpleCore::stopRealmInfrastructure(){
    if(_realmComponent->isRunning()){
        _realmComponent->stopComponent();
        return true;
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
}

void FKSimpleCore::createComponents(){
    _realmComponent=new FKRealmComponent(this);
    _serverComponent=new FKServerComponent(this);
    _clientComponent=new FKClientComponent(this);
}

void FKSimpleCore::installComponents(){
    connect(_realmComponent,SIGNAL(messageRequested(QString)),SIGNAL(messageRequested(QString)));
    connect(_serverComponent,SIGNAL(messageRequested(QString)),SIGNAL(messageRequested(QString)));
    connect(_clientComponent,SIGNAL(messageRequested(QString)),SIGNAL(messageRequested(QString)));
}

void FKSimpleCore::installComponentFactories(){
    _realmComponent->setComponentFactory(new FKFactoryObjectCreator<FKRealm>());
    _serverComponent->setComponentFactory(new FKFactoryObjectCreator<FKServerInfrastructure>());
    _clientComponent->setComponentFactory(new FKFactoryObjectCreator<FKClientInfrastructure>());
}

//void FKAbstractCore::setServer(FKServerInfrastructure* server){
//    _server=server;
//    connect(server,SIGNAL(waitingForAnswerChanged(FKInfrastructureType)),SLOT(waitingForAnswerChanged(FKInfrastructureType)));
//    connect(server,SIGNAL(connectedToRealm()),SLOT(serverConnectedToRealmSlot()));
//    connect(server,SIGNAL(disconnectedFromRealm()),SLOT(serverDisconnectedFromRealmSlot()));
//    connect(server,SIGNAL(loggedIn()),SLOT(serverLoggedInSlot()));
//    connect(server,SIGNAL(messageRequested(QString)),SIGNAL(messageRequested(QString)));
//    FKDataBase* db=new FKFSDB(_server);
//    db->setPath(serverDatabasePath());
//    _server->setDataBase(db);
//}

//void FKAbstractCore::setClientInfrastructure(FKClientInfrastructure* infr){
//    _infr=infr;
//    connect(infr,SIGNAL(waitingForAnswerChanged(FKInfrastructureType)),SLOT(waitingForAnswerChanged(FKInfrastructureType)));
//    connect(infr,SIGNAL(connectedToRealm()),SIGNAL(clientConnectedToRealm()));
//    connect(infr,SIGNAL(disconnectedFromRealm()),SIGNAL(clientDisconnectedFromRealm()));
//    connect(infr,SIGNAL(loggedIn()),SIGNAL(clientLoggedIn()));
//    connect(infr,SIGNAL(connectedToServer()),SIGNAL(clientConnectedToServer()));
//    connect(infr,SIGNAL(disconnectedFromServer()),SIGNAL(clientDisconnectedFromServer()));
//    connect(infr,SIGNAL(loggedInServer()),SIGNAL(clientLoggedInServer()));
//    connect(infr,SIGNAL(messageRequested(QString)),SIGNAL(messageRequested(QString)));
//    //connect(infr,SIGNAL(userPoolChanged()),SIGNAL(userPoolChanged()));
//    //connect(infr,SIGNAL(activeUsersChanged()),SIGNAL(userListChanged()));
//    //connect(infr,SIGNAL(customServerRequested(qint32,QString)),SLOT(createCustomServer(qint32,QString)));
//    connect(infr,SIGNAL(connectToServerRequest(QString,qint32)),SLOT(connectClientToServer(QString,qint32)));
//    connect(infr,SIGNAL(updateListChanged()),SIGNAL(clientGotUpdateList()));
//    connect(infr,SIGNAL(roomModuleChanged()),SIGNAL(uiSourceChanged()));
//    connect(infr,SIGNAL(roomModuleChanged()),SIGNAL(roomChanged()));
//}
