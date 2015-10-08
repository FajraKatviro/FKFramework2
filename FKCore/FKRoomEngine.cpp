#include "FKRoomEngine.h"

#include <QQmlComponent>

#include "FKLogger.h"

FKRoomEngine::FKRoomEngine(QObject *parent) : QObject(parent){
    FK_CBEGIN
    _userInfrastructureComponent=new QQmlComponent(qmlEngine(this),this);
    FK_CEND
}

FKRoomEngine::~FKRoomEngine(){
    FK_DBEGIN
    FK_DEND
}

