#include "FKRoomEngine.h"

#include <QQmlComponent>

#include "FKRoomModule.h"
#include "FKVersionList.h"
#include "FKMessage.h"
#include "FKRoomContext.h"
#include "FKRoomContextFlags.h"

#include "FKLogger.h"

FKRoomEngine::FKRoomEngine(QObject *parent) : QObject(parent){
    FK_CBEGIN
    _contextComponent=new QQmlComponent(qmlEngine(this),this);
    FK_CEND
}

FKRoomEngine::~FKRoomEngine(){
    FK_DBEGIN
    FK_DEND
}

QObject* FKRoomEngine::contextManager() const{
    return _contextManager;
}

FKRoomContext *FKRoomEngine::serverContext() const{
    return _contexts.value(_serverContextId,nullptr);
}

FKRoomContext *FKRoomEngine::userContext() const{
    return _contexts.value(_userContextId,nullptr);
}

QQmlListProperty<QObject> FKRoomEngine::roomContextItems(){
    return QQmlListProperty<QObject>(this,nullptr,&countContexts,&getContextItem);
}

void FKRoomEngine::loadModule(const QString moduleName){
    bool answer=false;
    if(!_roomModule){
        _roomModule=new FKRoomModule(this,moduleName);
        if(_roomModule->load()){
            QQmlComponent managerComponent(qmlEngine(this),QUrl(_roomModule->contextManager()));
            _contextManager=managerComponent.create(qmlContext(this));
            if(_contextManager){
                _contextManager->setProperty("roomEngine",QVariant::fromValue(this));
                emit contextManagerChanged();
                answer=true;
            }else{
                releaseModule();
            }
        }else{
            QString warn=QString(tr("Unable load room module %1")).arg(moduleName);
            FKMessage* msg=new FKMessage(warn);
            releaseModule();
            emit messageDispatched(msg);
        }
    }else{
        answer=true;
    }
    FKVersionList version = answer ? _roomModule->version() : FKVersionList();
    emit moduleLoaded(version);
}

void FKRoomEngine::releaseModule(){
    if(_roomModule){
        _roomModule->deleteLater();
        _roomModule=nullptr;
    }
    if(_contextManager){
        _contextManager->deleteLater();
        _contextManager=nullptr;
        emit contextManagerChanged();
    }
}

void FKRoomEngine::createContext(const qint32 rootId, qint8 flags){
    if(_roomModule){
        if(!_contexts.contains(rootId)){
            FKRoomContext* newContext;
            QObject* contextRootItem=_contextComponent->create(qmlContext(this));
            contextRootItem->setParent(newContext);
            newContext->setRootEntity(contextRootItem);
            contextRootItem->setProperty("roomContext",QVariant::fromValue(newContext));
            _contexts.insert(rootId,newContext);
            emit roomContextItemsChanged();
            if(flags & FKRoomContextFlag::server){
                _serverContextId=rootId;
                emit serverContextChanged();
            }
            if(flags & FKRoomContextFlag::user){
                _userContextId=rootId;
                emit userContextChanged();
            }
            emit contextCreated(rootId);
        }
    }
}

void FKRoomEngine::releaseContext(const qint32 rootId){

}

QObject* FKRoomEngine::getContextItem(QQmlListProperty<QObject>* prop, int index){
    FKRoomEngine* e=static_cast<FKRoomEngine*>(prop->object);
    qint32 i=0;
    for(auto it=e->_contexts.constBegin();it!=e->_contexts.constEnd();++it,++i){
        if(i==index)return (*it)->rootEntity();
    }
    return nullptr;
}

int FKRoomEngine::countContexts(QQmlListProperty<QObject>* prop){
    FKRoomEngine* e=static_cast<FKRoomEngine*>(prop->object);
    return e->_contexts.size();
}

