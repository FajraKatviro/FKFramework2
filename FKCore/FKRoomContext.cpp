#include "FKRoomContext.h"

#include "FKObjectManager.h"
#include "FKRoom.h"
#include "FKUser.h"

#include "FKLogger.h"

FKRoomContext::FKRoomContext(QObject *parent) : QObject(parent){
    FK_CBEGIN
    _om=new FKObjectManager(this);
    FK_CEND
}

FKRoomContext::~FKRoomContext(){
    FK_DBEGIN
    FK_DEND
}

FKObject* FKRoomContext::roomObject() const{
    return _room;
}

FKObject*FKRoomContext::userObject() const{
    return _user;
}

QObject*FKRoomContext::rootEntity() const{
    return _rootEntity;
}

bool FKRoomContext::isSyncComplete() const{
    return _syncComplete;
}

void FKRoomContext::setRootEntity(QObject* entity){
    if(_rootEntity!=entity){
        _rootEntity=entity;
        emit rootEntityChanged();
    }
}

bool FKRoomContext::addClient(const FKRoomInviteData &invite){
    todo;
}

void FKRoomContext::completeSync(){
    _syncComplete=true;
    emit syncCompleteChanged();
}

void FKRoomContext::onObjectCreated(FKObject* newObject){
    if(!_syncComplete){
        if(!_room){
            auto room=qobject_cast<FKRoom*>(newObject);
            if(room){
                _room=room;
                emit roomObjectChnaged();
                return;
            }
        }
        if(!_user && newObject->getId()==_userId){
            _user=qobject_cast<FKUser*>(newObject);
            emit userObjectChnaged();
            return;
        }
    }
}

void FKRoomContext::installObjectManager(FKObjectManager* om){

}

void FKRoomContext::eventRecycler(FKEventObject* ev){
    FK_MLOGV("Got undesired event",ev->subject())
    ev->deleteLater();
}

void FKRoomContext::installObjectManager(){
    installObjectManager(_om);
}

void FKRoomContext::setUser(const qint32 id){
    _userId=id;
}

