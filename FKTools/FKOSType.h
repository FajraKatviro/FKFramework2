#ifndef FKOSTYPE_H
#define FKOSTYPE_H

#include <QtGlobal>
#include <QString>

namespace FKOSType{
    const qint8 common=0;
    const qint8 nix=1;
    const qint8 ard=2;
    const qint8 mac=3;
    const qint8 ios=4;
    const qint8 win=5;
#if defined(Q_OS_WIN)
    const qint8 current=win;
#elif defined(Q_OS_IOS)
    const qint8 current=ios;
#elif defined(Q_OS_MAC)
    const qint8 current=mac;
#elif defined(Q_OS_ANDROID)
    const qint8 current=ard;
#elif defined(Q_OS_UNIX)
    const qint8 current=nix;
#endif
}

namespace FKOSDir{
    const QString nix=QStringLiteral("bin_nix");
    const QString ard=QStringLiteral("bin_android");
    const QString mac=QStringLiteral("bin_mac");
    const QString ios=QStringLiteral("bin_ios");
    const QString win=QStringLiteral("bin_win");
#if defined(Q_OS_WIN)
    const QString current=win;
#elif defined(Q_OS_IOS)
    const QString current=ios;
#elif defined(Q_OS_MAC)
    const QString current=mac;
#elif defined(Q_OS_ANDROID)
    const QString current=ard;
#elif defined(Q_OS_UNIX)
    const QString current=nix;
#endif
}

#endif // FKOSTYPE_H
