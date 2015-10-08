#include "FKThreadedComponent.h"

#include "FKFactory.h"

#include "FKLogger.h"

FKThreadedComponent::FKThreadedComponent(QObject *parent):QObject(parent){
    FK_CBEGIN
    FK_CEND
}

FKThreadedComponent::~FKThreadedComponent(){
    FK_DBEGIN
    delete _componentFactory;
    FK_DEND
}

void FKThreadedComponent::stopComponent(){
    if(isRunning()){
        connect(_componentObject,SIGNAL(destroyed(QObject*)),&_componentThread,SLOT(quit()));
        _componentObject->deleteLater();
        _componentObject=nullptr;
        _componentThread.wait();
    }
}

bool FKThreadedComponent::isRunning() const{
    return _componentThread.isRunning();
}

void FKThreadedComponent::setComponentFactory(FKAbstractFactoryObjectCreator* creator){
    delete _componentFactory;
    _componentFactory=creator;
}

void FKThreadedComponent::startComponent(QObject* object){
    _componentObject=object;
    _componentObject->moveToThread(&_componentThread);
    _componentThread.start();
}

QObject* FKThreadedComponent::component() const{
    return _componentObject;
}

