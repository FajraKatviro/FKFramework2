#include "FKRoomEngine.h"

#include <QQmlComponent>

#include "FKRoomModule.h"
#include "FKVersionList.h"
#include "FKEventObject.h"
#include "FKRoomContext.h"
#include "FKRoomContextFlags.h"

#include "FKInstructionSubjects.h"
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

FKVersionList FKRoomEngine::loadModule(const QString moduleName){
    bool answer=false;
    if(!_roomModule){
        _roomModule=new FKRoomModule(this,moduleName);
        if(_roomModule->load()){
            QQmlComponent managerComponent(qmlEngine(this),QUrl(_roomModule->contextManager()));
            _contextComponent->loadUrl(QUrl(_roomModule->contextComponent()));
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
            releaseModule();
            emit messageRequested(warn);
        }
    }else{
        answer=true;
    }
    FKVersionList version = answer ? _roomModule->version() : FKVersionList();
    return version;
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
    _contextComponent->loadUrl(QUrl());
}

FKRoomContext* FKRoomEngine::createContext(const qint32 rootId, qint8 flags){
    FKRoomContext* newContext=nullptr;
    if(_roomModule){
        if(!_contexts.contains(rootId)){
            newContext=_roomModule->createContextObject(this);
            QObject* contextInterface=_contextComponent->create(qmlContext(this));
            contextInterface->setParent(newContext);
            contextInterface->setProperty("roomContext",QVariant::fromValue(newContext));
            newContext->setRootEntity(contextInterface->property("rootEntity").value<QObject*>());
            _contexts.insert(rootId,newContext);
            connect(newContext,&FKRoomContext::messageRequested,&FKRoomEngine::messageRequested);
            connect(newContext,&FKRoomContext::instructionDispatched,&FKRoomEngine::instructionDispatched);
            emit roomContextItemsChanged();
            if(flags & FKRoomContextFlag::server){
                _serverContextId=rootId;
                emit serverContextChanged();
            }else{
                connect(newContext,&FKRoomContext::eventDispatched,&FKRoomEngine::cancelEvent);
                connect(newContext,&FKRoomContext::notificationDispatched,&FKRoomEngine::cancelEvent);
            }
            if(flags & FKRoomContextFlag::user){
                _userContextId=rootId;
                emit userContextChanged();
            }else{
                connect(newContext,&FKRoomContext::actionDispatched,&FKRoomEngine::cancelEvent);
            }
        }
    }
    return newContext;
}

void FKRoomEngine::releaseContext(const qint32 rootId){
    if(_roomModule){
        if(_contexts.contains(rootId)){
            FKRoomContext* oldContext=_contexts.take(rootId);
            emit roomContextItemsChanged();
            if( _serverContextId==rootId){
                _serverContextId=-1;
                emit serverContextChanged();
            }
            if(_userContextId==rootId){
                _userContextId=-1;
                emit userContextChanged();
            }
            oldContext->deleteLater();
        }
    }
}

void FKRoomEngine::processAction(FKEventObject *ev){
    QList<qint32> recievers(ev->recievers());
    for(auto i=recievers.constBegin();i!=recievers.constEnd();++i){
        FKRoomContext* roomContext=_contexts.value(*i,nullptr);
        if(roomContext){
            roomContext->processAction(ev);
        }
    }
    ev->deleteLater();
}

void FKRoomEngine::processEvent(FKEventObject *ev){
    QList<qint32> recievers(ev->recievers());
    for(auto i=recievers.constBegin();i!=recievers.constEnd();++i){
        FKRoomContext* roomContext=_contexts.value(*i,nullptr);
        if(roomContext){
            roomContext->processEvent(ev);
        }
    }
    ev->deleteLater();
}

void FKRoomEngine::processInstruction(FKInstructionObject instruction){
    qint32 subject=instruction->subject();
    if(subject==FKInstructionSubject::loadModule    ){
        todo;
    }else if(subject==FKInstructionSubject::releaseModule ){
        todo;
    }else if(subject==FKInstructionSubject::createContext ){
        todo;
    }else if(subject==FKInstructionSubject::releaseContext){
        todo;
    }else if(subject==FKInstructionSubject::createObjects ){
        todo;
    }else if(subject==FKInstructionSubject::deleteObjects ){
        todo;
    }else if(subject==FKInstructionSubject::addClient     ){
        todo;
    }else if(subject==FKInstructionSubject::removeClient  ){
        todo;
    }else if(subject==FKInstructionSubject::setRoomObject ){
        todo;
    }else if(subject==FKInstructionSubject::setUserObject ){
        todo;
    }else{
        todo;
    }
}

void FKRoomEngine::cancelEvent(QObject* ev){
    QString msg=QString(tr("Warning! Event object of type %1 was canceled").arg(ev->metaObject()->className()));
    messageRequested(msg);
    ev->deleteLater();
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

