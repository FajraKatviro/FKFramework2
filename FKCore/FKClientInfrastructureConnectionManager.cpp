#include "FKClientInfrastructureConnectionManager.h"

#include "FKClientInfrastructure.h"
#include "FKMessage.h"
#include "FKBasicEvent.h"

#include "FKLogger.h"
#include "FKBasicEventSubjects.h"

FKClientInfrastructureConnectionManager::FKClientInfrastructureConnectionManager(FKClientInfrastructure* infr, FKConnector* connector, QObject* parent)
        :FKConnectionManager(connector,parent),_infr(infr){
    FK_CBEGIN
    FK_CEND
}

FKClientInfrastructureConnectionManager::~FKClientInfrastructureConnectionManager(){
    FK_DBEGIN
    FK_DEND
}

void FKClientInfrastructureConnectionManager::processMessage(FKMessage* msg){
    _infr->messageFromRealm(msg->subject());
    msg->deleteLater();
}

void FKClientInfrastructureConnectionManager::processBasicEvent(FKBasicEvent* ev){
    const QString subject=ev->subject();
    const QVariant value=ev->value();
    ev->deleteLater();
    if(subject==FKBasicEventSubject::login){
        _infr->submitLoginRealm(value);
    }else if(subject==FKBasicEventSubject::userList){
        _infr->refreshUserList(value);
    }else if(subject==FKBasicEventSubject::createUser){
        _infr->respondUserCreation(value);
    }else if(subject==FKBasicEventSubject::deleteUser){
        _infr->respondUserDeletion(value);
    }else if(subject==FKBasicEventSubject::selectUser){
        _infr->respondUserAuthorization(value);
    }else if(subject==FKBasicEventSubject::deselectUser){
        _infr->respondUserDeauthorization(value);
    }else if(subject==FKBasicEventSubject::roomList){
        _infr->respondRoomList(value);
    }else if(subject==FKBasicEventSubject::customServer){
        _infr->respondCustomServer(value);
    }else if(subject==FKBasicEventSubject::createRoom){
        _infr->respondCreateRoom(value);
    }else{
        FK_MLOGV("Unexpected basic event from realm to client infrastructure",subject)
    }
}
