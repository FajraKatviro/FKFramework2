#include "FKUpdateChannel.h"

#include "FKOSType.h"
#include "FKPathResolver.h"

#include <QMap>
#include <QDir>

const qint64 FKUpdateChannel::blockSize=1024;

FKUpdateChannel::FKUpdateChannel(const QString& path, const QString& version, const qint8 platform, const QString& newVersion, const qint64 newSize)
        :_path(path),_oldVersion(version),_newVersion(newVersion),_platform(platform),_newSize(newSize){
    QFileInfo srcFile(FKPathResolver::roomFilePath(path,platform));
    QDir srcDir(srcFile.dir());
    srcDir.mkpath(".");
    _source.setFileName(srcFile.path());
    _oldSize=srcFile.size();

    srcFile=QFileInfo(FKPathResolver::roomFileDownloadPath(path,platform));
    srcDir=QDir(srcFile.dir());
    srcDir.mkpath(".");
    _target.setFileName(srcFile.path());

    if(_newVersion!=_oldVersion){
        if(!_oldVersion.isEmpty())_status|=FKUpdateChannelStatus::needDelete;
        if(!_newVersion.isEmpty())_status|=FKUpdateChannelStatus::needUpdate;
        if(_newSize>0)            _status|=FKUpdateChannelStatus::avaliable;
    }
}

FKUpdateChannel::~FKUpdateChannel(){}

FKUpdateChannel::ReadResult FKUpdateChannel::read(const qint32 reader){
    ReadResult result;
    if(_readers.isEmpty()){
        if(!_source.open(QIODevice::ReadOnly)){
            result.message=QString("Unable open file %1 to read update data").arg(_source.fileName());
            result.success=false;
            return result;
        }
    }
    QMap<qint32,qint64>::iterator r=_readers.find(reader);
    if(r==_readers.end())r=_readers.insert(reader,0);
    if(!_source.seek(r.value())){
        result.message=QString("Unable seek %2 file %1 to read update data").arg(_source.fileName()).arg(QString::number(r.value()));
        result.success=false;
        return result;
    }
    if(!_source.atEnd())result.data=_source.read(blockSize);
    result.finalBlock=_source.atEnd();
    if(result.finalBlock){
        _readers.erase(r);
    }else{
        r.value()=_source.pos();
    }
    result.finished=_readers.isEmpty();
    if(result.finished)_source.close();
    return result;
}

FKUpdateChannel::WriteResult FKUpdateChannel::write(const QByteArray& data, const bool final){
    WriteResult result;
    if(!_target.isOpen()){
        if(!_target.open(QIODevice::WriteOnly)){
            result.message=QString("Unable open file %1 to write update data").arg(_target.fileName());
            result.success=false;
            return result;
        }else{
            _status|=FKUpdateChannelStatus::startLoading;
        }
    }
    if(!data.isEmpty()){
        qint32 bytesWritten=_target.write(data);
        if(bytesWritten<0){
            result.message=QString("Unable write update data to %1 file").arg(_target.fileName());
            result.success=false;
            return result;
        }
    }
    if(final){
        _target.close();
        _status|=FKUpdateChannelStatus::finishLoading;
    }
    return result;
}

bool FKUpdateChannel::backupOld(const QString& storeId){
    if(!_readers.isEmpty())return false;
    if(_source.exists()){
        QString backupPath(FKPathResolver::roomFileBackupPath(_path,_platform,storeId));
        if(!_source.copy(backupPath)){
            return false;
        }
        _status|=FKUpdateChannelStatus::copied;
    }
    return true;
}

bool FKUpdateChannel::deleteOld(){
    if(!_readers.isEmpty())return false;
    if(_source.exists()){
        if(!_source.remove())return false;
        _status|=FKUpdateChannelStatus::deleted;
    }
    return true;
}

bool FKUpdateChannel::moveNew(){
    if(!_readers.isEmpty())return false;
    if(_target.exists()){
        if(!_target.rename(_source.fileName()))return false;
        _status|=FKUpdateChannelStatus::updated;
    }
    return true;
}
