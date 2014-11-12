#ifndef FKCOMMANDS_H
#define FKCOMMANDS_H

#include <QString>

namespace FKCommands {
    const QString quit=QStringLiteral("quit");
    const QString help=QStringLiteral("help");
    const QString startRealm=QStringLiteral("start realm");
    const QString startClient=QStringLiteral("start client");
    const QString createUser=QStringLiteral("create user");
    const QString deleteUser=QStringLiteral("delete user");
    const QString showUsers=QStringLiteral("show users");
    const QString loginClient=QStringLiteral("login client");
    const QString createClient=QStringLiteral("create client");
}

namespace FKCommandOptions {
    const QString realm=QStringLiteral("-r");
}

#endif // FKCOMMANDS_H
