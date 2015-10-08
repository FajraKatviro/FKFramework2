#include "FKPathResolver.h"

#include "FKOSType.h"

#include <QCoreApplication>

QString FKPathResolver::roomFilePath(const QString& path,const qint8 platform){
    return QString("%1/../%2/%3/%4").
            arg(QCoreApplication::applicationDirPath()).
            arg(FKOSDir::platformFolders.value(platform,FKOSDir::uncknown)).
            arg(FKPathResolver::roomsFolder).
            arg(path);
}

QString FKPathResolver::roomFileDownloadPath(const QString& path,const qint8 platform){
    return QString("%1/../%2/%3/%4/%5").
            arg(QCoreApplication::applicationDirPath()).
            arg(FKPathResolver::downloadFolder).
            arg(FKOSDir::platformFolders.value(platform,FKOSDir::uncknown)).
            arg(FKPathResolver::roomsFolder).
            arg(path);
}

QString FKPathResolver::roomFileBackupPath(const QString& path, qint8 platform, const QString& store){
    return QString("%1/../%2/%3/%4/%5/%6").
            arg(QCoreApplication::applicationDirPath()).
            arg(FKPathResolver::backupFolder).
            arg(store).
            arg(FKOSDir::platformFolders.value(platform,FKOSDir::uncknown)).
            arg(FKPathResolver::roomsFolder).
            arg(path);
}

QString FKPathResolver::realmDatabasePath(){
    return QString("%1/../%2/%3").
            arg(QCoreApplication::applicationDirPath()).
            arg(FKPathResolver::databaseFolder).
            arg(FKPathResolver::realmFolder);
}

QString FKPathResolver::serverDatabasePath(){
    return QString("%1/../%2/%3").
            arg(QCoreApplication::applicationDirPath()).
            arg(FKPathResolver::databaseFolder).
            arg(FKPathResolver::serverFolder);
}
