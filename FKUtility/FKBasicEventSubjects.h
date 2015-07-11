#ifndef FKBASICEVENTSUBJECTS_H
#define FKBASICEVENTSUBJECTS_H

#include <QString>

namespace FKBasicEventSubject{
    const QString login=QStringLiteral("lgn"); //guest autentification request
    const QString createRoom=QStringLiteral("cro"); //create room
    const QString startRoom=QStringLiteral("sro"); //create room
    const QString stopRoom=QStringLiteral("str"); //room desttoyed at server side
    const QString joinRoom=QStringLiteral("jr"); //join room request
    const QString quitRoom=QStringLiteral("qr"); //quit room message
    const QString registerRoomType=QStringLiteral("rrt"); //create room
    const QString roomList=QStringLiteral("rl"); //refresh avaliable rooms
    const QString roomData=QStringLiteral("rd"); //refresh existing room actual data
    const QString connect=QStringLiteral("cn"); //connection request
    const QString userList=QStringLiteral("ul"); //initially send userlist
    const QString createUser=QStringLiteral("cu"); //create new user record
    const QString deleteUser=QStringLiteral("du"); //delete existing user record
    const QString selectUser=QStringLiteral("su"); //add existing user to active list
    const QString deselectUser=QStringLiteral("dsu"); //remove existing user from active list
    const QString customServer=QStringLiteral("cus"); //create server for client's custom room
}

#endif // FKBASICEVENTSUBJECTS_H
