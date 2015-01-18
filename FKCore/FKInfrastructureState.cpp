#include "FKInfrastructureState.h"

#include "FKInfrastructureStateValue.h"

#include "FKLogger.h"

FKInfrastructureState::FKInfrastructureState(const QString stateName, QObject* parent):
        QObject(parent),_current(FKInfrastructureStateValueIdentifier::ValuesCount){
    FK_CBEGIN
    setObjectName(stateName);
    for(char i=0;i<FKInfrastructureStateValueIdentifier::ValuesCount;++i){
        _states[i]=new FKInfrastructureStateValue(this);
    }
    FK_CEND
}

FKInfrastructureState::~FKInfrastructureState(){
    FK_DBEGIN
    FK_DEND
}

bool FKInfrastructureState::accept(const qint32 val) const{
    if(!decline(val)){
        for(char i=0;i<FKInfrastructureStateValueIdentifier::ValuesCount;++i){
            if(_states[i]->accept(val))return true;
        }
    }
    return false;
}

bool FKInfrastructureState::decline(const qint32 val) const{
    for(char i=0;i<FKInfrastructureStateValueIdentifier::ValuesCount;++i){
        if(_states[i]->decline(val))return true;
    }
    return false;
}

void FKInfrastructureState::setState(const FKInfrastructureStateValueIdentifier state){
    if(state!=FKInfrastructureStateValueIdentifier::ValuesCount && state!=_current){
        _current=state;
        _states[state]->initState();
        emit stateChanged();
    }
}

void FKInfrastructureState::setState(const QString& childName, const FKInfrastructureStateValueIdentifier state){
    foreach(FKInfrastructureState* s,findChildren<FKInfrastructureState*>(childName)){
        s->setState(state);
    }
}

FKInfrastructureStateValue* FKInfrastructureState::state(FKInfrastructureStateValueIdentifier s){
    if(FKInfrastructureStateValueIdentifier::ValuesCount<=s)return 0;
    return _states[s];
}
