#ifndef FKROOMSETTINGS_H
#define FKROOMSETTINGS_H

//Identifiers used to access realm-sync room data

namespace FKRoomSettings{
    const qint32 isStarted=1;
    const qint32 isCustom=2;
    const qint32 isCreatedByUser=3;
    const qint32 server=4;
    const qint32 roomType=5;
    const qint32 owner=6;
    const qint32 creationTime=7;
    const qint32 maximumUsers=8;    //maximum users count allowed for room, including watchers
    const qint32 maximumActors=9;   //maximum actors (players) allowed for room
    const qint32 minimumActors=10;  //minimum actors (players) required for start
    const qint32 actorsToStart=11;  //minimum actors (players) required for auto-start
    const qint32 maximumWatchers=12;//maximum non-actor users allowed for room
    const qint32 neighbours=13;     //allow multiple users from single client
    const qint32 enemyNeighbours=14;//allow multiple users from single client act for enemy teams
    const qint32 totalUsers=15;     //info only
    const qint32 totalActors=16;    //info only
    const qint32 contextFlags=17;
    const qint32 userList=18;
    const qint32 customSettings=1000;
}

#endif // FKROOMSETTINGS_H
