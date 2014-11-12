#ifndef FKFILESLIST_H
#define FKFILESLIST_H

#include <QString>
#include <QMap>
#include <QFileInfo>
#include <QDateTime>

#include "FKOSType.h"

#include "fkcore_global.h"

class FKFileInfo{
public:
    FKFileInfo():_size(0){}
    FKFileInfo(const QString& path,const qint64 size,
               const QDateTime& date,const QByteArray& hash):_path(path),_size(size),_date(date),_hash(hash){}
    FKFileInfo(const FKFileInfo& other):_path(other._path),_size(other._size),_date(other._date),_hash(other._hash){}
    ~FKFileInfo(){}
    bool isValid()const{return _path!=QString();}
    QString path()const{return _path;}
    qint64 size()const{return _size;}
    QDateTime date()const{return _date;}
    QByteArray hash()const{return _hash;}
    bool operator==(const FKFileInfo& other)const{return (_path==other._path && _size==other._size && _date==other._date && _hash==other._hash);}
    bool operator!=(const FKFileInfo& other)const{return !(*this==other);}
private:
    QString _path;
    qint64 _size;
    QDateTime _date;
    QByteArray _hash;
};

class /*FKCORESHARED_EXPORT*/ FKFilesList{
public:
    FKFilesList(){}
    FKFilesList(const FKFilesList& other):_files(other._files){}
    ~FKFilesList(){}
    bool isEmpty()const;
    int add(const QFileInfo& source,const qint8 osType=FKOSType::current);
    int add(const FKFileInfo& source,const qint8 osType=FKOSType::current);
    int addRecursive(const QFileInfo& source,const qint8 osType=FKOSType::current);
    FKFilesList operator-(const FKFilesList& other)const;
    FKFilesList filterSinglePlatform(const qint8 osType=FKOSType::current)const;
    FKFilesList& operator+=(const FKFilesList& other);
    QMap<QString,FKFileInfo> getFilesInfo(const qint8 osType)const;
private:
    static FKFileInfo createFileInfo(const QFileInfo& file);
    QMap<qint8,QMap<QString,FKFileInfo> > _files;
};

#endif // FKFILESLIST_H
