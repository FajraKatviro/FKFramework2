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

QStringList FKRealmComponent::userList() const{
    QStringList lst;
    if(!FK_THREAD_GETTER(QStringList,lst,userList)){
        FK_MLOG("Unable get user list")
    }
    return lst;
}

QStringList FKRealmComponent::userList(const QString clientId) const{
    QStringList lst;
    if(!FK_THREAD_GETTER_ARG(QStringList,lst,getUserList,QString,clientId)){
        FK_MLOG("Unable get user list")
    }
    return lst;
}

QStringList FKRealmComponent::clientList() const{
    QStringList lst;
    if(!FK_THREAD_GETTER(QStringList,lst,clientList)){
        FK_MLOG("Unable get client list")
    }
    return lst;
}

QStringList FKRealmComponent::connectedClientList() const{
    QStringList lst;
    if(!FK_THREAD_GETTER(QStringList,lst,connectedClientList)){
        FK_MLOG("Unable get client list")
    }
    return lst;
}

QStringList FKRealmComponent::activeClientList() const{
    QStringList lst;
    if(!FK_THREAD_GETTER(QStringList,lst,connectedClientList)){
        FK_MLOG("Unable get client list")
    }
    return lst;
}

QStringList FKRealmComponent::serverList() const{
    QStringList lst;
    if(!FK_THREAD_GETTER(QStringList,lst,serverList)){
        FK_MLOG("Unable get server list")
    }
    return lst;
}

QStringList FKRealmComponent::serverList(const QString roomType) const{
    QStringList lst;
    if(!FK_THREAD_GETTER_ARG(QStringList,lst,getServersForRoomType,QString,roomType)){
        FK_MLOG("Unable get server list")
    }
    return lst;
}

QList<qint32> FKRealmComponent::connectedServerList() const{
    QList<qint32> lst;
    if(!FK_THREAD_GETTER(QList<qint32>,lst,connectedServerList)){
        FK_MLOG("Unable get server list")
    }
    return lst;
}

QStringList FKRealmComponent::roomTypeList() const{
    QStringList lst;
    if(!FK_THREAD_GETTER(QStringList,lst,getRegisteredRoomTypes)){
        FK_MLOG("Unable get room type list")
    }
    return lst;
}

QStringList FKRealmComponent::roomTypeList(const qint32 serverId) const{
    QStringList lst;
    if(!FK_THREAD_GETTER_ARG(QStringList,lst,serverAvaliableRoomTypes,qint32,serverId)){
        FK_MLOG("Unable get room type list")
    }
    return lst;
}

QStringList FKRealmComponent::activeRoomList() const{
    return activeRoomList(QVariant());
}

QStringList FKRealmComponent::activeRoomList(const QVariant filter) const{
    QStringList lst;
    if(!FK_THREAD_GETTER_ARG(QStringList,lst,getRoomList,QVariant,filter)){
        FK_MLOG("Unable get room list")
    }
    return lst;
}

void FKRealmComponent::setPort(const qint32 port){
    FK_THREAD_CALL_ARG(setPort,qint32,port);
}

void FKRealmComponent::guestConnection(FKConnector* connector){
    FK_THREAD_CALL_ARG(incomeConnection,FKConnector*,connector);
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
