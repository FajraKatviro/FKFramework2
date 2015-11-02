#include "FKThreadedComponent.h"

#include "FKFactory.h"

#include "FKLogger.h"

/*!
\class FKThreadedComponent
\brief Provides threaded component access api
*/

FKThreadedComponent::FKThreadedComponent(QObject *parent):QObject(parent){
    FK_CBEGIN
    FK_CEND
}

FKThreadedComponent::~FKThreadedComponent(){
    FK_DBEGIN
    _componentThread.quit();
    _componentThread.wait();
    delete _componentObject;
    delete _componentFactory;
    FK_DEND
}

void FKThreadedComponent::stopComponent(){
    if(isRunning()){
        if(_stopRequested)return;
        connect(_componentObject,SIGNAL(destroyed(QObject*)),SLOT(componentThreadQuit()));
        _componentObject->deleteLater();
        _componentObject=nullptr;
        _stopRequested=true;
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

void FKThreadedComponent::componentThreadQuit(){
    _stopRequested=false;
    _componentThread.quit();
    _componentThread.wait();
}

