#ifndef FKPATHRESOLVER_H
#define FKPATHRESOLVER_H

#include <QString>

#include "fkcore_global.h"

//Used to determine different application used paths

namespace FKPathResolver{
    const QString downloadFolder("download");
    const QString backupFolder("backup");
    const QString roomsFolder("rooms");
    const QString realmFolder("realm");
    const QString serverFolder("server");
    const QString databaseFolder("data");

    QString FKCORESHARED_EXPORT roomFilePath(const QString& path, const qint8 platform);
    QString FKCORESHARED_EXPORT roomFileDownloadPath(const QString& path, const qint8 platform);
    QString FKCORESHARED_EXPORT roomFileBackupPath(const QString& path, qint8 platform, const QString& store);
    QString FKCORESHARED_EXPORT realmDatabasePath();
    QString FKCORESHARED_EXPORT serverDatabasePath();
    QString FKCORESHARED_EXPORT databasePath();
}

#endif // FKPATHRESOLVER_H
