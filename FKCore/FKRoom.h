#ifndef FKROOM_H
#define FKROOM_H

#include "FKObject.h"

class FKRoomData;

enum class FKRoomSettings{
    MaximumUsers,   //maximum users count allowed for room, including watchers
    MaximumActors,  //maximum actors (players) allowed for room
    MinimumActors,  //minimum actors (players) required for start
    ActorsToStart,  //minimum actors (players) required for auto-start
    MaximumWatchers,//maximum non-actor users allowed for room
    Neighbours,     //allow multiple users from single client
    EnemyNeighbours,//allow multiple users from single client act for enemy teams
    CustomSettings=1000
};

class FKCORESHARED_EXPORT FK_ABSTRACT_OBJECT(FKRoom,FKObject)
    Q_OBJECT
public:
    virtual void setup(const FKRoomData& data);
protected:
    inline QVariant getSetting(FKRoomSettings setting)const{return _roomSettings.value(setting);}
    inline void setSetting(FKRoomSettings setting,const QVariant& value){_roomSettings[setting]=value;}
private:
    QMap<FKRoomSettings,QVariant> _roomSettings;
};


#endif // FKROOM_H
