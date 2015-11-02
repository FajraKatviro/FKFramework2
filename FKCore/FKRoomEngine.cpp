#include "FKRoomEngine.h"

#include <QQmlComponent>

#include "FKRoomModule.h"
#include "FKVersionList.h"
#include "FKBasicEvent.h"
#include "FKEventObject.h"
#include "FKRoomContext.h"
#include "FKRoomContextFlags.h"
#include "FKRoomInviteData.h"

#include "FKInstructionSubjects.h"
#include "FKLogger.h"

/*!
\class FKRoomEngine
\brief Component incapsulates room contexts management
*/

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
            newContext=_roomModule->createContextObject();
            newContext->setParent(this);
            QObject* contextInterface=_contextComponent->create(qmlContext(this));
            contextInterface->setParent(newContext);
            contextInterface->setProperty("roomContext",QVariant::fromValue(newContext));
            newContext->setRootEntity(contextInterface->property("rootEntity").value<QObject*>());
            _contexts.insert(rootId,newContext);
            connect(newContext,&FKRoomContext::messageRequested,this,&FKRoomEngine::messageRequested);
            emit roomContextItemsChanged();
            if(flags & FKRoomContextFlag::server){
                _serverContextId=rootId;
                emit serverContextChanged();
            }else{
                connect(newContext,&FKRoomContext::eventDispatched,this,&FKRoomEngine::cancelEvent);
                connect(newContext,&FKRoomContext::notificationDispatched,this,&FKRoomEngine::cancelEvent);
            }
            if(flags & FKRoomContextFlag::user){
                _userContextId=rootId;
                emit userContextChanged();
            }else{
                connect(newContext,&FKRoomContext::actionDispatched,this,&FKRoomEngine::cancelEvent);
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
    qint32 subject=instruction.subject();

    if(      subject==FKInstructionSubject::loadModule    ){
        loadModuleInstruction(instruction);

    }else if(subject==FKInstructionSubject::releaseModule ){
        releaseModuleInstruction(instruction);

    }else if(subject==FKInstructionSubject::createContext ){
        createContextInstruction(instruction);

    }else if(subject==FKInstructionSubject::releaseContext){
        releaseContextInstruction(instruction);

    }else if(subject==FKInstructionSubject::createObject ){
        createObjectInstruction(instruction);

    }else if(subject==FKInstructionSubject::deleteObject ){
        deleteObjectInstruction(instruction);

    }else if(subject==FKInstructionSubject::addClient     ){
        addClientInstruction(instruction);

    }else if(subject==FKInstructionSubject::removeClient  ){
        removeClientInstruction(instruction);

    }else if(subject==FKInstructionSubject::setRoomObject ){
        setRoomInstruction(instruction);

    }else if(subject==FKInstructionSubject::setUserObject ){
        setUserInstruction(instruction);

    }else{
        QString msg=QString(tr("Warning! Invalid instruction provided of type %1")).arg(QString::number(instruction.subject()));
        emit messageRequested(msg);
    }
}

void FKRoomEngine::cancelEvent(QObject* ev){
    QString msg=QString(tr("Warning! Event object of type %1 was canceled")).arg(ev->metaObject()->className());
    emit messageRequested(msg);
    ev->deleteLater();
}

void FKRoomEngine::loadModuleInstruction(const FKInstructionObject &instruction){
    FKVersionList moduleVersion=loadModule(instruction.value().toString());
    if(!moduleVersion.isValid())moduleVersion=loadDefaultModule();
    FKInstructionObject result=FKInstructionObject(FKInstructionSubject::loadModule,QVariant::fromValue(moduleVersion));
    emit instructionDispatched(result);
}

void FKRoomEngine::releaseModuleInstruction(const FKInstructionObject &instruction){
    releaseModule();
    emit instructionDispatched(instruction);
}

void FKRoomEngine::createContextInstruction(const FKInstructionObject &instruction){
    QPair<qint32,qint8> data=instruction.value().value<QPair<qint32,qint8>>();
    FKRoomContext* newContext=createContext(data.first,data.second);
    FKInstructionObject result=FKInstructionObject(FKInstructionSubject::createContext,QVariant::fromValue(newContext));
    emit instructionDispatched(result);
}

void FKRoomEngine::releaseContextInstruction(const FKInstructionObject &instruction){
    releaseContext(instruction.value().toInt());
    emit instructionDispatched(instruction);
}

void FKRoomEngine::createObjectInstruction(const FKInstructionObject &instruction){
    QList<qint32> recievers(instruction.recievers());
    for(auto i=recievers.constBegin();i!=recievers.constEnd();++i){
        FKRoomContext* contextObject=_contexts.value(*i,nullptr);
        if(contextObject)contextObject->createObject(instruction.value());
    }
}

void FKRoomEngine::deleteObjectInstruction(const FKInstructionObject &instruction){
    QList<qint32> recievers(instruction.recievers());
    for(auto i=recievers.constBegin();i!=recievers.constEnd();++i){
        FKRoomContext* contextObject=_contexts.value(*i,nullptr);
        if(contextObject)contextObject->deleteObject(instruction.value());
    }
}

void FKRoomEngine::addClientInstruction(const FKInstructionObject &instruction){
    QList<qint32> recievers(instruction.recievers());
    QList<qint32> succeed, failed;
    for(auto i=recievers.constBegin();i!=recievers.constEnd();++i){
        FKRoomContext* contextObject=_contexts.value(*i,nullptr);
        if(contextObject){
            FKRoomInviteData inviteData(instruction.value());
            bool clientAdded=contextObject->addClient(inviteData);
            if(clientAdded){
                succeed.append(*i);
            }else{
                failed.append(*i);
            }
        }else{
            failed.append(*i);
        }
    }
    if(!failed.isEmpty()){
        FKInstructionObject failResult(failed,FKInstructionSubject::addClient,false);
        emit instructionDispatched(failResult);
    }
    if(!succeed.isEmpty()){
        FKInstructionObject successResult(succeed,FKInstructionSubject::addClient,true);
        emit instructionDispatched(successResult);
    }
}

void FKRoomEngine::removeClientInstruction(const FKInstructionObject &instruction){
    QList<qint32> recievers(instruction.recievers());
    QList<qint32> succeed, failed;
    for(auto i=recievers.constBegin();i!=recievers.constEnd();++i){
        FKRoomContext* contextObject=_contexts.value(*i,nullptr);
        if(contextObject){
            FKRoomInviteData inviteData(instruction.value());
            contextObject->removeClient(inviteData);
            succeed.append(*i);
        }else{
            failed.append(*i);
        }
    }
    if(!failed.isEmpty()){
        FKInstructionObject failResult(failed,FKInstructionSubject::removeClient,false);
        emit instructionDispatched(failResult);
    }
    if(!succeed.isEmpty()){
        FKInstructionObject successResult(succeed,FKInstructionSubject::removeClient,true);
        emit instructionDispatched(successResult);
    }
}

void FKRoomEngine::setRoomInstruction(const FKInstructionObject &instruction){
    QList<qint32> recievers(instruction.recievers());
    QList<qint32> succeed, failed;
    qint32 objectId=instruction.value().toInt();
    for(auto i=recievers.constBegin();i!=recievers.constEnd();++i){
        FKRoomContext* contextObject=_contexts.value(*i,nullptr);
        if(contextObject){
            contextObject->setRoomObject(objectId);
            succeed.append(*i);
        }else{
            failed.append(*i);
        }
    }
    if(!failed.isEmpty()){
        FKInstructionObject failResult(failed,FKInstructionSubject::setRoomObject,false);
        emit instructionDispatched(failResult);
    }
    if(!succeed.isEmpty()){
        FKInstructionObject successResult(succeed,FKInstructionSubject::setRoomObject,true);
        emit instructionDispatched(successResult);
    }
}

void FKRoomEngine::setUserInstruction(const FKInstructionObject &instruction){
    QList<qint32> recievers(instruction.recievers());
    QList<qint32> succeed, failed;
    qint32 objectId=instruction.value().toInt();
    for(auto i=recievers.constBegin();i!=recievers.constEnd();++i){
        FKRoomContext* contextObject=_contexts.value(*i,nullptr);
        if(contextObject){
            contextObject->setUserObject(objectId);
            succeed.append(*i);
        }else{
            failed.append(*i);
        }
    }
    if(!failed.isEmpty()){
        FKInstructionObject failResult(failed,FKInstructionSubject::setUserObject,false);
        emit instructionDispatched(failResult);
    }
    if(!succeed.isEmpty()){
        FKInstructionObject successResult(succeed,FKInstructionSubject::setUserObject,true);
        emit instructionDispatched(successResult);
    }
}

FKVersionList FKRoomEngine::loadDefaultModule(){
    return loadModule("FKChatRoom");
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

