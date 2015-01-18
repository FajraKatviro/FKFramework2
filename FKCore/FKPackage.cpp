#include "FKPackage.h"

#include "FKLogger.h"

#include <QFile>

//these are bit-flags
qint8 FKPackageObject::Resource::cacheEnabled=1;
qint8 FKPackageObject::Resource::cached=2;

FKPackageObject::FKPackage(const QString& source):_enabled(false),_source(source){
    if(source.isEmpty()){
        FK_MLOG("Warning! Invalid package object constructed")
    }
}

FKPackageObject::FKPackage(const FKPackageObject& other):_offset(other._offset),_enabled(other._enabled),_source(other._source),_resources(other._resources){
    if(_source.isEmpty()){
        FK_MLOG("Warning! Invalid package object copied")
    }
}

bool FKPackageObject::setEnabled(const bool t){
    if(t!=_enabled){
        if(t){
            QVariantMap data=readData(0).toMap();
            for(auto i=data.constBegin();i!=data.constEnd();++i){
                if(_resources.contains(i.key())){
                    FK_MLOGV("Warning! Resource already exists",i.key())
                    continue;
                }
                qint64 resourceOffset=i.value().toLongLong();
                if(!resourceOffset){
                    FK_MLOGV("Warning! Invalid resource offset",i.key())
                    continue;
                }
                _resources.insert(i.key(),FKPackageResource(new Resource(resourceOffset)));
            }
        }else{
            _resources.clear();
        }
    }
}

void FKPackageObject::setCacheEnabled(const bool enabled){
    if(enabled){
        for(auto i=_resources.begin();_resources.end();++i){
            i.value()->setCacheEnabled();
        }
    }else{
        for(auto i=_resources.begin();_resources.end();++i){
            i.value()->setCacheDisabled();
        }
    }
}

void FKPackageObject::setCacheEnabled(const QString& resource, const bool enabled){
    auto i=_resources.find(resource);
    if(i!=_resources.end()){
        if(enabled){
            i.value()->setCacheEnabled();
        }else{
            i.value()->setCacheDisabled();
        }
    }else{
        FK_MLOGV("Resource not found for cache enableing",resource)
    }
}

void FKPackageObject::preload(){
    for(auto i=_resources.begin();_resources.end();++i){
        if(i.value()->isCacheEnabled()){
            i.value()->preload();
        }else{
            FK_MLOGV("Unable preload resource if cache is not enabled",i.key())
        }
    }
}

void FKPackageObject::preload(const QString& resource){
    auto i=_resources.find(resource);
    if(i!=_resources.end()){
        if(i.value()->isCacheEnabled()){
            i.value()->preload();
        }else{
            FK_MLOGV("Unable preload resource if cache is not enabled",i.key())
        }
    }else{
        FK_MLOGV("Resource not found for preload",resource)
    }
}

void FKPackageObject::clearCache(){
    for(auto i=_resources.begin();_resources.end();++i){
        i.value()->clearCache();
    }
}

void FKPackageObject::clearCache(const QString& resource){
    auto i=_resources.find(resource);
    if(i!=_resources.end()){
        i.value()->clearCache();
    }else{
        FK_MLOGV("Resource not found for cache clearing",resource)
    }
}

QVariant FKPackageObject::resourceData(const QString& resource){
    auto target=_resources.find(resource);
    if(target==_resources.end()){
        FK_MLOGV("resource not found",resource)
        return QVariant();
    }

    if(target.value()->isCached()){
        return target.value()->value;
    }

    if(!target.value()->offset){
        FK_MLOGV("Warning! Unable get resource offset",target.value().value)
        return QVariant();
    }

    QVariant data=readData(target.value()->offset);

    if(target.value()->isCacheEnabled()){
        target.value()->value=data;
        target.value()->setCached();
    }

    return data;
}

QVariant FKPackageObject::readData(const qint64 offset){
    QVariant data;
    QFile file(_source);
    if(file.open(QIODevice::ReadOnly)){
        file.seek(offset);
        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_5_3);
        stream>>data;
    }else{
        FK_MLOGV("unable read resource file",_source)
    }
    return data;
}
