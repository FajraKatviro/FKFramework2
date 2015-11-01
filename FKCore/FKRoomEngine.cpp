#include "FKRoomEngine.h"

#include <QQmlComponent>

#include "FKRoomModule.h"
#include "FKVersionList.h"
#include "FKEventObject.h"
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
            FKRoomContext* newContext=nullptr; todo; //create here
            QObject* contextInterface=_contextComponent->create(qmlContext(this));
            contextInterface->setParent(newContext);
            contextInterface->setProperty("roomContext",QVariant::fromValue(newContext));
            newContext->setRootEntity(contextInterface->property("rootEntity").value<QObject*>());
            _contexts.insert(rootId,newContext);
            emit roomContextItemsChanged();
            if(flags & FKRoomContextFlag::server){
                _serverContextId=rootId;
//                connect(newContext, &FKRoomContext::actionDispatched,       &FKRoomEngine::cancelEvent,            Qt::UniqueConnection);
//                connect(newContext, &FKRoomContext::eventDispatched,        &FKRoomEngine::eventDispatched,        Qt::UniqueConnection);
//                connect(newContext, &FKRoomContext::messageDispatched,      &FKRoomEngine::messageDispatched,      Qt::UniqueConnection);
//                connect(newContext, &FKRoomContext::instructionDispatched,  &FKRoomEngine::cancelEvent,            Qt::UniqueConnection);
//                connect(newContext, &FKRoomContext::notificationDispatched, &FKRoomEngine::notificationDispatched, Qt::UniqueConnection);
                emit serverContextChanged();
            }
            if(flags & FKRoomContextFlag::user){
                _userContextId=rootId;
//                connect(newContext, &FKRoomContext::actionDispatched,       &FKRoomEngine::actionDispatched,       Qt::UniqueConnection);
//                connect(newContext, &FKRoomContext::eventDispatched,        &FKRoomEngine::cancelEvent,            Qt::UniqueConnection);
//                connect(newContext, &FKRoomContext::messageDispatched,      &FKRoomEngine::messageDispatched,      Qt::UniqueConnection);
//                connect(newContext, &FKRoomContext::instructionDispatched,  &FKRoomEngine::instructionDispatched,  Qt::UniqueConnection);
//                connect(newContext, &FKRoomContext::notificationDispatched, &FKRoomEngine::cancelEvent,            Qt::UniqueConnection);
                emit userContextChanged();
            }
            emit contextCreated(rootId);
        }
    }

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

void FKRoomEngine::processInstruction(FKInstructionObject *instruction){
    FKRoomContext* roomContext=_contexts.value(instruction->reciever(),nullptr);
    if(roomContext){
        roomContext->processInstruction(instruction);
    }
    instruction->deleteLater();
}

void FKRoomEngine::cancelEvent(QObject* ev){
    FKMessage* msg=new FKMessage(QString(tr("Warning! Event object of type %1 was canceled").arg(ev->metaObject()->className())));
    messageDispatched(msg);
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

