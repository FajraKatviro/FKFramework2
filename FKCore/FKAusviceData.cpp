#include "FKAusviceData.h"

#include <QObject>
#include <QVariant>
#include <QMap>

/*!
\class FKAusviceData
\brief This class incapsulates login data for any guest connectiond
*/

FKAusviceData::FKAusviceData():_valid(false){
    _error=QString(QObject::tr("Empty ausvice data"));
}

FKAusviceData::FKAusviceData(const QString& clientId, const QString& password):
            _infrastructureType(FKAusviceIdentifiers::client),_serverId(-1),
            _clientId(clientId),_password(password),_valid(true){
    if(clientId.isEmpty()){
        _valid=false;
        _error=QString(QObject::tr("empty client id"));
    }else if(password.isEmpty()){
        _valid=false;
        _error=QString(QObject::tr("empty password"));
    }else if(!isClientName(clientId)){
        _valid=false;
        _error=QString(QObject::tr("invalid client name"));
    }
}

FKAusviceData::FKAusviceData(const qint32 serverId, const QString& password):
            _infrastructureType(FKAusviceIdentifiers::server),_serverId(serverId),
            _password(password),_valid(true){
    if(serverId<=0){
        _valid=false;
        _error=QString(QObject::tr("invalid server id"));
    }else if(password.isEmpty()){
        _valid=false;
        _error=QString(QObject::tr("empty password"));
    }
}

FKAusviceData::FKAusviceData(const QVariant& data):_valid(false){
    QMap<QString,QVariant> map=data.toMap();
    if(map.isEmpty()){
        _error=QString(QObject::tr("invalid data container"));
        return;
    }

    const QString sourceType=map.take(FKAusviceIdentifiers::infrastructureType).toString();
    if(sourceType!=FKAusviceIdentifiers::client && sourceType!=FKAusviceIdentifiers::server){
        _error=QString(QObject::tr("invalid ausvice identifier"));
        return;
    }
    _infrastructureType=sourceType;

    const QString password=map.take(FKAusviceIdentifiers::password).toString();
    if(password.isEmpty()){
        _error=QString(QObject::tr("password not provided"));
        return;
    }
    _password=password;

    const QVariant id=map.take(FKAusviceIdentifiers::id);
    if(!map.isEmpty()){
        _error=QString(QObject::tr("too many arguments provided"));
        return;
    }

    if(sourceType==FKAusviceIdentifiers::client){
        const QString strId=id.toString();
        if(strId.isEmpty()){
            _error=QString(QObject::tr("invalid client id"));
            return;
        }

        if(!isClientName(strId)){
            _error=QString(QObject::tr("invalid client name %1")).arg(strId);
            return;
        }
        _clientId=strId;

    }else{
        const qint32 intId=id.toInt();
        if(intId<=0){
            _error=QString(QObject::tr("invalid server id"));
            return;
        }
        _serverId=intId;

    }
    _valid=true;
}

QVariant FKAusviceData::toVariant() const{
    if(_valid){
        QMap<QString,QVariant> data;
        data[FKAusviceIdentifiers::infrastructureType]=_infrastructureType;
        data[FKAusviceIdentifiers::password]=_password;
        if(FKAusviceIdentifiers::client==_infrastructureType){
            data[FKAusviceIdentifiers::id]=_clientId;
        }else if(FKAusviceIdentifiers::server==_infrastructureType){
            data[FKAusviceIdentifiers::id]=_serverId;
        }
        return data;
    }else{
        return QVariant();
    }
}

bool FKAusviceData::isValid() const{
    return _valid;
}

QString FKAusviceData::error() const{
    return _error;
}

bool FKAusviceData::isClient() const{
    return _infrastructureType==FKAusviceIdentifiers::client;
}

QString FKAusviceData::clientId() const{
    return _clientId;
}

qint32 FKAusviceData::serverId() const{
    return _serverId;
}

QString FKAusviceData::password() const{
    return _password;
}

bool FKAusviceData::isClientName(const QString& val){
    return QRegExp("[0-9a-zA-Z]+").exactMatch(val);
}
