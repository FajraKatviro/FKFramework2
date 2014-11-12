#include "FKFilesList.h"

#include "FKLogger.h"

#include <QDir>
#include <QCryptographicHash>
#include <QCoreApplication>

/*!
 * \class FKFileInfo
 * \brief Class used to represent identification information of single file. The instancies only contains given information and can not check or modify it
 */

/*!
 * \fn FKFileInfo::FKFileInfo()
 * \brief Constructs empty invalid object
 */

/*!
 * \fn FKFileInfo::FKFileInfo(const QString& path,const qint64 size,const QDateTime& date,const QByteArray& hash)
 * \brief Constructs object with given parametrs
 */

/*!
 * \fn FKFileInfo::FKFileInfo(const FKFileInfo& other)
 * \brief Constructs copy of other
 */

/*!
 * \fn FKFileInfo::~FKFileInfo()
 * \brief Simply destructs object
 */

/*!
 * \fn FKFileInfo::isValid()const
 * \brief Returns true if path is valid string
 */

/*!
 * \fn QString FKFileInfo::path()const
 * \brief Returns given file path (expected path relative to application execution file)
 */

/*!
 * \fn qint64 FKFileInfo::size()const
 * \brief Returns given file size
 */

/*!
 * \fn QDateTime date()const
 * \brief Returns given date (expected last modification date time)
 */

/*!
 * \fn QByteArray FKFileInfo::hash()const
 * \brief Returns given hash-code (expected sha1 of file)
 */

/*!
 * \fn bool FKFileInfo::operator==(const FKFileInfo& other)const
 * \brief Compare all components of objects and return true if equal
 */

/*!
 * \fn bool FKFileInfo::operator!=(const FKFileInfo& other)const
 * \brief Compare all components of objects and return false if equal
 */

/*!
 * \class FKFilesList
 * \brief Class represents map of FKFileInfo objects for different osTypes. Should be used for versionalizing application
 */

/*!
 * \fn FKFilesList::FKFilesList()
 * \brief Constructs empty object
 */

/*!
 * \fn FKFilesList::FKFilesList(const FKFilesList& other)
 * \brief Constructs a copy of other
 */

/*!
 * \fn FKFilesList::~FKFilesList()
 * \brief Simply destructs object
 */

/*!
 * \brief Returns true if object do not contains any FKFileInfo instancies
 */

bool FKFilesList::isEmpty()const{
    for(auto i=_files.constBegin();i!=_files.constEnd();++i){
        if(!i.value().isEmpty())return false;
    }
    return true;
}

/*!
 * \brief Add file or files from folder for given osType role
 */

int FKFilesList::add(const QFileInfo& source, const qint8 osType){
    int count=0;
    if(source.exists()){
        if(source.isFile()){
            count+=add(createFileInfo(source),osType);
        }else if(source.isDir()){
            foreach(QFileInfo i, source.absoluteDir().entryInfoList(QDir::Files)){
                count+=add(i,osType);
            }
        }
    }
    return count;
}

/*!
 * \brief Add source FKFileInfo object for given osType role
 */

int FKFilesList::add(const FKFileInfo& source,const qint8 osType){
    _files[osType][source.path()]=source;
    return 1;
}

/*!
 * \brief Add file or all files from folder and subfolders for given osType role
 */

int FKFilesList::addRecursive(const QFileInfo& source, const qint8 osType){
    int count=0;
    if(source.exists()){
        if(source.isFile()){
            count+=add(createFileInfo(source),osType);
        }else if(source.isDir()){
            foreach(QFileInfo i, source.absoluteDir().entryInfoList(QDir::NoDotAndDotDot)){
                count+=addRecursive(i,osType);
            }
        }
    }
    return count;
}

/*!
 * \brief Substruct equal elements of other
 */

FKFilesList FKFilesList::operator-(const FKFilesList& other)const{
    FKFilesList lst(*this);
    for(auto i=other._files.constBegin();i!=other._files.constEnd();++i){
        const QMap<QString,FKFileInfo> source=i.value();
        QMap<QString,FKFileInfo> target=lst._files.value(i.key());
        for(auto j=source.constBegin();j!=source.constEnd();++j){
            if(target.contains(j.key()) && target.value(j.key())==j.value()){
                target.remove(j.key());
            }
        }
        lst._files[i.key()]=target;
    }
    return lst;
}

/*!
 * \brief Returns files list containing elements for common and given roles only. Default role is current OS type
 */

FKFilesList FKFilesList::filterSinglePlatform(const qint8 osType)const{
    FKFilesList lst;
    for(auto i=_files.constBegin();i!=_files.constEnd();++i){
        if(i.key()==FKOSType::common || i.key()==osType){
            lst._files.insert(i.key(),i.value());
        }
    }
    return lst;
}

/*!
 * \brief Append all non-equal elements of other
 */

FKFilesList& FKFilesList::operator+=(const FKFilesList& other){
    for(auto i=other._files.constBegin();i!=other._files.constEnd();++i){
        const QMap<QString,FKFileInfo> source=i.value();
        QMap<QString,FKFileInfo> target=_files.value(i.key());
        for(auto j=source.constBegin();j!=source.constEnd();++j){
            if(!target.contains(j.key()) || target.value(j.key())!=j.value()){
                target.insert(j.key(),j.value());
            }
        }
        _files[i.key()]=target;
    }
    return *this;
}

/*!
 * \brief Return map of elements for given role
 */

QMap<QString,FKFileInfo> FKFilesList::getFilesInfo(const qint8 osType)const{
    return _files.value(osType);
}

FKFileInfo FKFilesList::createFileInfo(const QFileInfo &file){
    qint64 size=file.size();
    QString path=QDir(qApp->applicationDirPath()).relativeFilePath(file.absoluteFilePath());
    QDateTime date=file.lastModified();
    QFile f(file.absoluteFilePath());
    QCryptographicHash h(QCryptographicHash::Sha1);
    if(!h.addData(&f)){
        FK_MLOGV("cannot get file data hash",file.absoluteFilePath())
    }
    QByteArray hash=h.result();
    return FKFileInfo(path,size,date,hash);
}
