#ifndef FKROOMLOADER_H
#define FKROOMLOADER_H

#include <QObject>

class FKPackageManager;

class FKRoomLoader{
public:
    FKRoomLoader();
    virtual ~FKRoomLoader(){}
    virtual QString version()const=0;
    virtual QString className()const=0;
    virtual void registerObjects()const=0;
    virtual void unregisterObjects()const=0;
    virtual FKPackageManager* createPackageManager()=0;
};

#define FKRoomLoader_IID "rooms.plugins"
Q_DECLARE_INTERFACE(FKRoomLoader,FKRoomLoader_IID)

#endif // FKROOMLOADER_H
