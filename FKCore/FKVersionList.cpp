#include "FKVersionList.h"

#include "FKPathResolver.h"

#include <QVariant>
#include <QFileInfo>


bool FKVersionString::operator>(const FKVersionString& other) const{
    QList<qint32> mLst(numbers()), oLst(other.numbers());
    for(qint32 i=0;i<4;++i){
        qint32 m=mLst.at(i), o=oLst.at(i);
        if(m>o)return true;
        if(m<o)return false;
    }
    return false;
}

bool FKVersionString::operator<(const FKVersionString& other) const{
    QList<qint32> mLst(numbers()), oLst(other.numbers());
    for(qint32 i=0;i<4;++i){
        const qint32 &m=mLst.at(i), &o=oLst.at(i);
        if(m>o)return false;
        if(m<o)return true;
    }
    return false;
}

bool FKVersionString::operator==(const FKVersionString& other) const{
    return _str==other._str;
}

bool FKVersionString::operator!=(const FKVersionString& other) const{
    return _str!=other._str;
}

QList<qint32> FKVersionString::numbers()const{
    QList<qint32> lst;
    QStringList versions(other.string().split('.'));
    qint32 size=versions.size();
    for(qint32 i=0;i<4;++i){
        lst.append(i < size ? versions.at(i).toInt() : 0);
    }
    return lst;
}


bool FKVersionList::isValid()const{
    return !_enteties.isEmpty();
}

void FKVersionList::addFile(const QString& path, const QString& version, const qint8 platform){
    QFileInfo f(FKPathResolver::roomFilePath(path,platform));
    _enteties.append(FKVersionList::Data(path,version,platform,f.size()));
}

void FKVersionList::addFile(const QString& path, const QString& version, const QList<qint8>& platforms){
    foreach(auto platform,platforms)addFile(path,version,platform);
}

const FKVersionString FKVersionList::actual()const{
    FKVersionString version;
    for(auto f=_enteties.constBegin();f!=_files.constEnd();++f){
        if((*f).version>version)
            version=(*f).version;
    }
    return version;
}

const FKVersionList FKVersionList::operator-(const FKVersionList& other)const{
    FKVersionList result(*this);
    for(auto o=other._enteties.constBegin();o!=other._enteties.constEnd();++o){
        auto n=result._enteties.begin();
        while(n!=result._enteties.end()){
            if((*n).path    ==(*o).path     &&
               (*n).platform==(*o).platform &&
               (*n).version ==(*o).version){
                n=result._enteties.erase(n);
            }else{
                ++n;
            }
        }
    }
    return result;
}


QDataStream& operator<<(QDataStream& stream, const FKVersionList& lst){
    QStringList pathPool, versionPool;
    QList<qint32> paths, versions;
    QList<qint8> platforms;
    QList<qint64> sizes;

    for(auto e=lst._enteties.constBegin();e!=lst._enteties.constEnd();++e){
        qint32 path=pathPool.indexOf(e->path), version=versionPool.indexOf(e->version.string());
        if(path<0){
            path=pathPool.size();
            pathPool.append(e->path);
        }
        if(version<0){
            version=versionPool.size();
            versionPool.append(e->version.string());
        }
        paths.append(path);
        versions.append(version);
        platforms.append(e->platform);
        sizes.append(e->size);
    }

    return stream<<pathPool<<versionPool<<paths<<versions<<platforms<<sizes;
}

QDataStream& operator>>(QDataStream& stream, FKVersionList& lst){
    QStringList pathPool, versionPool;
    QList<qint32> paths, versions;
    QList<qint8> platforms;
    QList<qint64> sizes;

    stream>>pathPool>>versionPool>>paths>>versions>>platforms>>sizes;

    if(pathPool.isEmpty()   ||
       versionPool.isEmpty()||
       platforms.isEmpty()  ||
       sizes.isEmpty())
            return stream;

    qint32 length=paths.size();
    if(versions.size()  != length ||
       platforms.size() != length ||
       sizes.size()     != length)
            return stream;

    qint32 lastPath=pathPool.size();
    qint32 lastVersion=versionPool.size();

    auto p=paths.constBegin();
    auto v=versions.constBegin();
    auto l=platforms.constBegin();
    auto s=sizes.constBegin();

    do{
        if((*p)<0 || (*p)>=lastPath ||
           (*v)<0 || (*v)>=lastVersion){
                lst._enteties.clear();
                return stream;
        }
        lst._enteties.append(FKVersionList::Data(pathPool.at(*p),FKVersionString(versionPool.at(*v)),*l,*s));
        ++p; ++v; ++l; ++s;
    }while(p!=paths.constEnd());

    return stream;
}
