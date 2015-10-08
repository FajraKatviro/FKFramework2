#include "FKUserInfrastructure.h"

#include "FKConnectionManager.h"

#include "FKBasicEventSubjects.h"
#include "FKLogger.h"

FKUserInfrastructure::FKUserInfrastructure(const qint32 id,FKWorld* worldObject,QObject* parent)
        :FKInfrastructure(parent),_world(worldObject),_id(id){
    FK_CBEGIN
    //_im=new FKInteractiveModel(this);
    FK_CEND
}

FKUserInfrastructure::~FKUserInfrastructure(){
    FK_DBEGIN
    FK_DEND
}

FKInfrastructureType FKUserInfrastructure::infrastructureType() const{
    return FKInfrastructureType::User;
}

void FKUserInfrastructure::createObjectRequest(qint32 reciever, QVariant request){
    todo;

}

void FKUserInfrastructure::deleteObjectRequest(qint32 reciever, QVariant request){
    todo;
}

void FKUserInfrastructure::incomeEvent(qint32 reciever, FKEventObject* event){
    todo;
}

//void FKUserInfrastructure::requestUpdate(const QStringList& targets){
//    if(!_logged){
//        emit messageRequested(QString(tr("Unable request update data: user is not logged")));
//    }
//    if(!_updateTargets.isEmpty()){
//        emit messageRequested(QString(tr("Unable request update data: another update in progress")));
//    }
//    if(!requestAnswer(FKInfrastructureType::Server,FKBasicEventSubject::update)){
//        FK_MLOG("Unexpected behaivour in FKUserInfrastructure::requestUpdate()")
//    }

//    qint32 index=0;
//    foreach(auto src,targets){
//        _updateTargets.insert(++index,QSharedPointer<FKUpdateChannel>::create(src));
//        FKUpdateData data(src,index);
//        todo;
//        FKBasicEvent request(FKBasicEventSubject::update,data.toVariant());
//        _serverConnection->sendBasicEvent(&request);
//    }
//}

//void FKUserInfrastructure::recieveUpdateData(const QVariant& data){
//    FKUpdateData updateData(data);
//    auto u=_updateTargets.find(updateData.target());
//    if(u!=_updateTargets.end()){
//        todo;
//    }else{
//        FK_MLOG("Got invalid update data index")
//    }
//}
