#ifndef FKROOMCOMPONENTS
#define FKROOMCOMPONENTS

#include "FKPackageManager.h"
#include "FKRoomLoader.h"

#define FK_ROOM_MODULE(RoomClass,Version,PackageManagerClass,UsedObjects...) \
class RoomClass ## Loader:public QObject,FKRoomLoader{\
    Q_OBJECT\
    Q_PLUGIN_METADATA(IID FKRoomLoader_IID)\
    Q_INTERFACES(FKRoomLoader)\
public:\
    virtual QString version()const{return Version;}\
    virtual QString className()const{return #RoomClass;}\
    virtual void registerObjects()const{FKSystemObject::allowConstructing<RoomClass>();FKSystemObject::allowConstructing<UsedObjects>();}\
    virtual void unregisterObjects()const{FKSystemObject::forbidConstructing<RoomClass>();FKSystemObject::forbidConstructing<UsedObjects>();}\
    virtual FKPackageManager* createPackageManager(){return new PackageManagerClass(this);}\
};

#define FK_PACKAGE_MANAGER(PackageManagerClass) \
class PackageManagerClass:public FKPackageManager{\
    Q_OBJECT\
    void initializePackages();\
public:\
    PackageManagerClass(QObject* parent=0):FKPackageManager(parent){}\
};\
\
inline void PackageManagerClass::initializePackages()

#endif // FKROOMCOMPONENTS

