#include "FKRoomModule.h"

#include "FKPackageManager.h"
#include "FKPackage.h"
#include "FKRoomLoader.h"

#include "FKLogger.h"

FKRoomModule::FKRoomModule(const QString& module,QObject* parent):QObject(parent),_loaded(false),_module(module),_room(0){
    FK_CBEGIN
    FK_CEND
}

FKRoomModule::~FKRoomModule(){
    FK_DBEGIN
    if(_room)_room->unregisterObjects();
    _loader.unload();
    FK_DEND
}

bool FKRoomModule::load(){
    if(!_loaded)_loaded=loadRoomPlugin();
    return _loaded;
}

QString FKRoomModule::version() const{
    return _room->version();
}

QString FKRoomModule::roomClass() const{
    return _room->className();
}

FKPackage FKRoomModule::getPackage(const QString& package){
    return _packageManager->getPackage(package);
}

QVariant FKRoomModule::resourceData(const QString& package, const QString& resource){
    return _packageManager->getPackage(package)->resourceData(resource);
}

void FKRoomModule::loadPackageManager(){
    _packageManager=_room->createPackageManager();
    _packageManager->initializeManager();
}

bool FKRoomModule::loadRoomPlugin(){
    _loader.setFileName(_module);
    _room=qobject_cast<FKRoomLoader*>(_loader.instance());
    if(!_room){
        FK_MLOGV("Warning! unable load room plugin",_module)
        return false;
    }
    _room->registerObjects();
    loadPackageManager();
    return true;
}
