#ifndef FKROOM_H
#define FKROOM_H

#include "FKObject.h"

class FKRoomData;
class FKRoomInviteData;

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

class FKCORESHARED_EXPORT FKRoom:public FKObject{
    FK_ABSTRACT_OBJECT(FKRoom,FKObject)
    Q_OBJECT
    Q_PROPERTY(qint32 o_usersSize READ ro_usersSize WRITE wo_usersSize)
public:
    virtual void setup(const FKRoomData& data);
    bool addUsers(const FKRoomInviteData& data);
    qint32 usersSize()const;
protected:
    inline QVariant getSetting(FKRoomSettings setting)const{return _roomSettings.value(setting);}
    inline void setSetting(FKRoomSettings setting,const QVariant& value){_roomSettings[setting]=value;}
signals:
    void roomDataChanged(const qint32 maxActorsDelta, const qint32 actorsDelta, const qint32 maxUsersDelta, const qint32 usersDelta);
private:
    qint32 ro_usersSize()const;
    void wo_usersSize(const qint32 size);

    QMap<FKRoomSettings,QVariant> _roomSettings;
    qint32 _usersSize;
};


#endif // FKROOM_H
