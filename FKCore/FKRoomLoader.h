#ifndef FKROOMLOADER_H
#define FKROOMLOADER_H

#include <QObject>

class FKPackageManager;

class FKRoomLoader{
public:
    FKRoomLoader();
    virtual ~FKRoomLoader()=0{}
    virtual QString version()const=0;
    virtual QString className()const=0;
    virtual void registerObjects()const=0;
    virtual void unregisterObjects()const=0;
    virtual FKPackageManager* createPackageManager()=0;
};

#define FKRoomLoader_IID "rooms.plugins"
Q_DECLARE_INTERFACE(FKRoomLoader,FKRoomLoader_IID)

#define FK_ROOM_MODULE(RoomClass,Version,PackageManagerClass,UsedObject1,...) \
class RoomClass ## Loader:public QObject,FKRoomLoader{\
    Q_OBJECT\
    Q_PLUGIN_METADATA(IID FKRoomLoader_IID)\
    Q_INTERFACES(FKRoomLoader)\
    virtual QString version()const{return Version;}\
    virtual QString className()const{return #RoomClass;}\
    virtual void registerObjects()const{FKSystemObject::allowConstructing<RoomClass>();regClasses();}\
    virtual void unregisterObjects()const{FKSystemObject::forbidConstructing<RoomClass>();unregClasses();}\
    virtual FKPackageManager* createPackageManager(){return new PackageManagerClass(this);}\
    void regClasses()const;\
    void unregClasses()const;\
};\
inline void RoomClass ## Loader::regClasses()const\
    FKSystemObject::allowConstructing<UsedObject1>();\
}\
inline void RoomClass ## Loader::unregClasses()const\
    FKSystemObject::forbidConstructing<UsedObject1>();\
}

#endif // FKROOMLOADER_H
