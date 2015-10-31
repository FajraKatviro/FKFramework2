#include "FKRoomModule.h"

#include "FKPackageManager.h"
#include "FKPackage.h"
#include "FKRoomLoader.h"
#include "FKVersionList.h"

#include "FKLogger.h"

QMutex FKRoomModule::listMutex;
QMap<QString,qint32> FKRoomModule::loadedModules;

FKRoomModule::FKRoomModule(QObject* parent, const QString& module):QObject(parent),_loaded(false),_module(module),_room(0){
    FK_CBEGIN
    FK_CEND
}

FKRoomModule::~FKRoomModule(){
    FK_DBEGIN
    QMutexLocker lock(&listMutex);
    if(_room && !--loadedModules[_module]){
        _room->unregisterObjects();
        loadedModules.remove(_module);
    }
    _loader.unload();
    FK_DEND
}

bool FKRoomModule::load(){
    if(!_loaded)_loaded=loadRoomPlugin();
    return _loaded;
}

FKVersionList FKRoomModule::version() const{
    return _room->version();
}

QString FKRoomModule::roomClass() const{
    return _room->className();
}

QString FKRoomModule::contextManager() const{
    return "qrc:/RoomContextManager.qml";
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
    QMutexLocker lock(&listMutex);
    if(!loadedModules.contains(_module)){
        _room->registerObjects();
        loadedModules.insert(_module,1);
    }else{
        ++loadedModules[_module];
    }
    loadPackageManager();
    return true;
}
