#ifndef FKCOMMANDS_H
#define FKCOMMANDS_H

#include <QString>

namespace FKCommands {
    const QString quit=QStringLiteral("quit");
    const QString help=QStringLiteral("help");
    const QString startRealm=QStringLiteral("start realm");
    const QString stopRealm=QStringLiteral("stop realm");
    const QString startClient=QStringLiteral("start client");
    const QString startServer=QStringLiteral("start server");
    const QString createUser=QStringLiteral("create user");
    const QString deleteUser=QStringLiteral("delete user");
    const QString showUsers=QStringLiteral("show users");
    const QString showRoomTypes=QStringLiteral("show room types");
    const QString showServers=QStringLiteral("show servers");
    const QString loginClient=QStringLiteral("login client");
    const QString loginServer=QStringLiteral("login server");
    const QString createClient=QStringLiteral("create client");
    const QString deleteClient=QStringLiteral("delete client");
    const QString createServer=QStringLiteral("create server");
    const QString deleteServer=QStringLiteral("delete server");
    const QString joinRoom=QStringLiteral("join room");
    const QString createRoom=QStringLiteral("create room");
    const QString registerRoomType=QStringLiteral("add room type");
    const QString removeRoomType=QStringLiteral("remove room type");
}

namespace FKCommandOptions {
    const QString realm=QStringLiteral("-r");
    const QString custom=QStringLiteral("-c");
    const QString server=QStringLiteral("-s");
}

#endif // FKCOMMANDS_H
