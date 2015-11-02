#ifndef FKBASICEVENTSUBJECTS_H
#define FKBASICEVENTSUBJECTS_H

#include <QString>

//Identifiers used for FKBasicEvent to specify requested command

namespace FKBasicEventSubject{
    const QString login=QStringLiteral("lgn"); //guest autentification request
    const QString createRoom=QStringLiteral("cro"); //create room
    //const QString startRoom=QStringLiteral("sro"); //create room
    const QString stopRoom=QStringLiteral("str"); //room desttoyed at server side
    const QString joinRoom=QStringLiteral("jr"); //join room request
    const QString quitRoom=QStringLiteral("qr"); //quit room message
    const QString registerRoomType=QStringLiteral("rrt");
    const QString removeRoomType=QStringLiteral("drt");
    const QString roomTypeList=QStringLiteral("rtl");
    const QString roomList=QStringLiteral("rl"); //refresh avaliable rooms
    const QString roomData=QStringLiteral("rd"); //refresh existing room actual data
    const QString connect=QStringLiteral("cn"); //connection request
    const QString sync=QStringLiteral("sc"); //client-server syncronization complete
    const QString userList=QStringLiteral("ul"); //send userlist to client
    const QString createUser=QStringLiteral("cu"); //create new user record
    const QString deleteUser=QStringLiteral("du"); //delete existing user record
    const QString version=QStringLiteral("vr"); //send or request version list
    const QString dropClient=QStringLiteral("dc"); //remove client from server's room
    //const QString selectUser=QStringLiteral("su"); //add existing user to active list
    //const QString deselectUser=QStringLiteral("dsu"); //remove existing user from active list
    //const QString customServer=QStringLiteral("cus"); //create server for client's custom room
    //const QString rejectCustomServer=QStringLiteral("rcs"); //client reply if unable start custom server by realm request
}

#endif // FKBASICEVENTSUBJECTS_H
