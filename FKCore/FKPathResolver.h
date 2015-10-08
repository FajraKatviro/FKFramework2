#ifndef FKPATHRESOLVER_H
#define FKPATHRESOLVER_H

#include <QString>

namespace FKPathResolver{
    const QString downloadFolder("download");
    const QString backupFolder("backup");
    const QString roomsFolder("rooms");
    const QString realmFolder("realm");
    const QString serverFolder("server");
    const QString databaseFolder("data");

    QString roomFilePath(const QString& path, const qint8 platform);
    QString roomFileDownloadPath(const QString& path, const qint8 platform);
    QString roomFileBackupPath(const QString& path, qint8 platform, const QString& store);
    QString realmDatabasePath();
    QString serverDatabasePath();
}

#endif // FKPATHRESOLVER_H
