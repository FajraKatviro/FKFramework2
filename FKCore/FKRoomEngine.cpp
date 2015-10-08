#include "FKRoomEngine.h"

#include <QQmlComponent>

#include "FKLogger.h"

FKRoomEngine::FKRoomEngine(QObject *parent) : QObject(parent){
    FK_CBEGIN
    _userContextComponent=new QQmlComponent(qmlEngine(this),this);
    FK_CEND
}

FKRoomEngine::~FKRoomEngine(){
    FK_DBEGIN
    FK_DEND
}

FKRoomContext *FKRoomEngine::serverContext() const{
    return _serverContext;
}

FKRoomContext *FKRoomEngine::defaultContext() const{
    return _userContexts.value(_defaultContext,nullptr);
}

QQmlListProperty<FKRoomContext> FKRoomEngine::userContexts() const{
    todo;
    return QQmlListProperty<FKRoomContext>();
}

