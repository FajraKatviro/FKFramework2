#ifndef FKROOMCOMPONENTS
#define FKROOMCOMPONENTS

#include "FKPackageManager.h"
#include "FKRoomLoader.h"
#include "FKRoom.h"


#define FK_ROOM_MODULE(RoomClass,Version,PackageManagerClass,UsedObjects...) \
class RoomClass ## Loader:public QObject,public FKRoomLoader{\
    Q_OBJECT\
    Q_PLUGIN_METADATA(IID FKRoomLoader_IID)\
    Q_INTERFACES(FKRoomLoader)\
public:\
    virtual QString version()const override{return #Version;}\
    virtual QString className()const override{return #RoomClass;}\
    virtual void registerObjects()const override{FKObject::allowConstructing<RoomClass,##UsedObjects>();}\
    virtual void unregisterObjects()const override{FKObject::forbidConstructing<RoomClass,##UsedObjects>();}\
    virtual FKPackageManager* createPackageManager()override{return new PackageManagerClass(this);}\
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

