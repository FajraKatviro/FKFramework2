#include "FKClientInfrastructureConnectionManager.h"

#include "FKClientInfrastructure.h"
#include "FKMessage.h"
#include "FKBasicEvent.h"
#include "FKEventObject.h"

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
    }else if(subject==FKBasicEventSubject::roomList){
        _infr->respondRoomList(value);
    }else if(subject==FKBasicEventSubject::createRoom){
        _infr->respondCreateRoom(value);
    }else if(subject==FKBasicEventSubject::joinRoom){
        _infr->respondEnterRoom(value);
    }else{
        FK_MLOGV("Unexpected basic event from realm to client infrastructure",subject)
    }
}

FKClientInfrastructureConnectionManagerS::FKClientInfrastructureConnectionManagerS(FKClientInfrastructure* infr, FKConnector* connector, QObject* parent)
        :FKConnectionManager(connector,parent),_infr(infr){
    FK_CBEGIN
    FK_CEND
}

FKClientInfrastructureConnectionManagerS::~FKClientInfrastructureConnectionManagerS(){
    FK_DBEGIN
    FK_DEND
}

void FKClientInfrastructureConnectionManagerS::processBasicEvent(FKBasicEvent* ev){
    const QString subject=ev->subject();
    const QVariant value=ev->value();
    ev->deleteLater();
    if(subject==FKBasicEventSubject::login){
        _infr->submitLoginRealm(value);
    }else if(subject==FKBasicEventSubject::version){
        _infr->incomeVersionData(value);
    }else{
        FK_MLOGV("Unexpected basic event from server to client infrastructure",subject)
    }
}

void FKClientInfrastructureConnectionManagerS::processEvent(FKEventObject* ev){
    FK_MLOGV("event processing not implemented",ev->subject())
    ev->deleteLater();
    todo;
}

void FKClientInfrastructureConnectionManagerS::processUpdateData(FKUpdateData* data){
    _infr->incomeUpdateData(data);
}

//void FKClientInfrastructureConnectionManagerS::processMessage(FKMessage* msg){
//    _infr->message(msg->subject());
//    msg->deleteLater();
//}
