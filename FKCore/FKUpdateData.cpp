#include "FKUpdateData.h"

#include "FKLogger.h"

#include <QVariant>

FKUpdateData::FKUpdateData():FKSystemObject(){
    FK_CBEGIN
    FK_CEND
}

FKUpdateData::~FKUpdateData(){
    FK_DBEGIN
    FK_DEND
}

FKUpdateData::FKUpdateData(const QString& src, const qint32 index)
        :FKSystemObject(),_index(index),_src(src){
    FK_CBEGIN
    FK_CEND
}

FKUpdateData::FKUpdateData(const qint32 index, const QByteArray& data, const bool final)
        :FKSystemObject(),_index(index),_data(data),_final(final){
    FK_CBEGIN
    FK_CEND
}

bool FKUpdateData::isFinal() const{
    return _final;
}

QString FKUpdateData::path() const{
    return _path;
}

qint32 FKUpdateData::index() const{
    return _index;
}

bool FKUpdateData::packObject(QDataStream& stream) const{
    stream<<_index<<
            _path<<
            _final<<
            _data;
    return true;
}

bool FKUpdateData::loadObject(QDataStream& stream){
    stream>>_index>>
            _path>>
            _final>>
            _data;
    return true;
}
