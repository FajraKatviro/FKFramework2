#ifndef FKUPDATECHANNEL_H
#define FKUPDATECHANNEL_H

#include "FKUpdateChannelStatus.h"

#include <QFile>

class FKUpdateChannel{
public:
    FKUpdateChannel(const QString& path,const QString& version, const qint8 platform, const QString& newVersion=QString(), const qint64 newSize=0);
    ~FKUpdateChannel();
    
    inline void setApplyEnabled(const bool t){_applyEnabled=t;}

    struct ReadResult{
        QByteArray data;
        bool finalBlock=false;
        bool finished=false;
        bool success=true;
        QString message;
    };
    struct WriteResult{
        bool success=true;
        QString message;
    };
    
    ReadResult read(const qint32 reader);
    WriteResult write(const QByteArray& data, const bool final);

    bool backupOld(const QString& storeId);
    bool deleteOld();
    bool moveNew();
    
    inline qint8 platform()const{return _platform;}
    inline QString path()const{return _path;}
    inline QString oldVersion()const{return _oldVersion;}
    inline QString newVersion()const{return _newVersion;}
    inline qint64 newSize()const{return _newSize;}
    inline qint64 oldSize()const{return _oldSize;}
    inline bool isApplyEnabled()const{return _applyEnabled;}
    inline qint8 status()const{return _status;}
private:
    QString _path;
    QString _oldVersion;
    QString _newVersion;
    QFile _source;
    QFile _target;
    qint8 _platform;
    qint64 _newSize=0;
    qint64 _oldSize;
    bool _applyEnabled=false;
    QMap<qint32,qint64> _readers;
    quint8 _status=FKUpdateChannelStatus::actual;

    static const qint64 blockSize;
};

#endif // FKUPDATECHANNEL_H
