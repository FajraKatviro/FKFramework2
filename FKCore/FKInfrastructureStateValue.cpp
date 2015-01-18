#include "FKInfrastructureStateValue.h"

#include "FKInfrastructureState.h"

#include "FKLogger.h"

FKInfrastructureStateValue::FKInfrastructureStateValue(QObject *parent) :
        QObject(parent),_initialized(false){
    FK_CBEGIN
    FK_CEND
}

FKInfrastructureStateValue::~FKInfrastructureStateValue(){
    FK_DBEGIN
    FK_DEND
}

void FKInfrastructureStateValue::initState(){
    if(!_initialized){
        _initialized=true;
        emit initRequested(this);
    }
}

bool FKInfrastructureStateValue::accept(const qint32 value) const{
    if(!decline(value)){
        if(_accept.contains(value))return true;
        foreach(FKInfrastructureState* subState,_substates){
            if(subState->accept(value))return true;
        }
    }
    return false;
}

bool FKInfrastructureStateValue::decline(const qint32 value) const{
    if(_decline.contains(value))return true;
    foreach(FKInfrastructureState* subState,_substates){
        if(subState->decline(value))return true;
    }
    return false;
}

void FKInfrastructureStateValue::addSubstate(FKInfrastructureState* subState){
    _substates.append(subState);
}

void FKInfrastructureStateValue::addDecline(const qint32 val){
    _decline.append(val);
}

void FKInfrastructureStateValue::addAccept(const qint32 val){
    _accept.append(val);
}
