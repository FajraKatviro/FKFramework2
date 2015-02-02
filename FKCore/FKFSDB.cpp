#include "FKFSDB.h"

#include "FKLogger.h"

#include <QFile>
#include <QDataStream>

FKFSDB::FKFSDB(QObject *parent):FKDataBase(parent){
    FK_CBEGIN
    FK_CEND
}

FKFSDB::~FKFSDB(){
    FK_DBEGIN
    FK_DEND
}

void FKFSDB::setPath(const QString& path){
    FKDataBase::setPath(path);
    _root=QDir(path);
}

void FKFSDB::reset(){
    if(_root.removeRecursively()){
        _root.mkdir(".");
    }else{
        FK_MLOGV("unable reset FSDB path",_root.canonicalPath())
    }
}

bool FKFSDB::hasIndex(const FKDBIndex& index) const{
    if(!index.isValid())return true;
    return _root.exists(index.sequentialPath().join('/'));
}

void FKFSDB::removeIndex(const FKDBIndex& index){
    if(index.isValid()){
        QDir target(_root);
        if(target.cd(index.sequentialPath().join('/'))){
            if(target.removeRecursively())return;
        }
        FK_MLOGV("Unable remove index",index.sequentialPath().join('/'))
    }else{
        reset();
    }
}

FKDBIndex FKFSDB::findIndex(const FKDBValue& value, const FKDBIndex& parentIndex,bool recursive) const{
    QDir dir(getIndexDirectory(parentIndex));
    foreach (QString nodeName,dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        dir.cd(nodeName);
        FKDBValue val=readValueFromDirectory(dir);
        if(recursive && val.valueType()==FKDBValue::Index)val=getValue(val.index(),true);
        if(val==value){
            FKDBIndex index(parentIndex);
            return index>>nodeName;
        }
        dir.cdUp();
    }
    return FKDBIndex();
}

qint32 FKFSDB::countValues(const FKDBIndex& ind) const{
    return getProperties(ind).count();
}

QList<FKDBValue> FKFSDB::getValues(const FKDBIndex& parentIndex) const{
    QDir dir(getIndexDirectory(parentIndex));
    QList<FKDBValue> lst;
    foreach (QString nodeName,dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        dir.cd(nodeName);
        lst.append(readValueFromDirectory(dir));
        dir.cdUp();
    }
    return lst;
}

QStringList FKFSDB::getProperties(const FKDBIndex& parentIndex) const{
    return getIndexDirectory(parentIndex).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}

QMap<QString, FKDBValue> FKFSDB::mapValues(const FKDBIndex& parentIndex) const{
    QDir dir(getIndexDirectory(parentIndex));
    QMap<QString, FKDBValue> lst;
    foreach (QString nodeName,dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        dir.cd(nodeName);
        lst.insert(nodeName,readValueFromDirectory(dir));
        dir.cdUp();
    }
    return lst;
}

FKDBValue FKFSDB::retriveValue(const FKDBIndex& index) const{
    return readValueFromDirectory(getIndexDirectory(index));
}

void FKFSDB::writeValue(const FKDBValue& value, const FKDBIndex& index) const{
    writeValueToDirectory(value,getIndexDirectory(index));
}

FKDBValue FKFSDB::readValueFromDirectory(const QDir& path) const{
    FKDBValue val;
    QFileInfo f(path,FKFSDB_VALUE);
    if(f.exists()){
        QFile file(f.absoluteFilePath());
        if(file.open(QIODevice::ReadOnly)){
            QDataStream stream(&file);
            stream.setVersion(QDataStream::Qt_5_3);
            stream>>val;
        }else{
            FK_MLOGV("unable open file",f.absoluteFilePath())
        }
    }else{
        FK_MLOGV("requested path does not exists",path.canonicalPath())
    }
    return val;
}

void FKFSDB::writeValueToDirectory(const FKDBValue& value, const QDir& path) const{
    if(path.mkpath(".")){
        QFile file(path.filePath(FKFSDB_VALUE));
        if(file.open(QIODevice::WriteOnly)){
            QDataStream stream(&file);
            stream.setVersion(QDataStream::Qt_5_3);
            stream<<value;
        }else{
            FK_MLOGV("unable open file to write",file.fileName())
        }
    }else{
        FK_MLOGV("unable create path for write",path.canonicalPath())
    }
}

QDir FKFSDB::getIndexDirectory(const FKDBIndex& index) const{
    QStringList path(index.sequentialPath());
    path.prepend(_root.canonicalPath());
    return QDir(path.join('/'));
}
