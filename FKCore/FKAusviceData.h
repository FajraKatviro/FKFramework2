#ifndef FKAUSVICEDATA_H
#define FKAUSVICEDATA_H

#include "FKAusviceIdentifiers.h"

class QVariant;

class FKAusviceData{
public:
    FKAusviceData();
    FKAusviceData(const QString& clientId,const QString& password);
    FKAusviceData(const qint32 serverId,const QString& password);
    FKAusviceData(const QVariant& data);
    QVariant toVariant()const;
    bool isValid()const;
    QString error()const;
    bool isClient()const;

    QString clientId()const;
    qint32 serverId()const;
    QString password()const;
private:
    static bool isClientName(const QString& val);
    QString _infrastructureType;
    qint32 _serverId;
    QString _clientId;
    QString _password;
    QString _error;
    bool _valid;
};

#endif // FKAUSVICEDATA_H
