#include "FKRoomDataFilter.h"

#include <QVariant>

FKRoomDataFilter::FKRoomDataFilter(const QVariant& filterData){
    const QMap<QString,QVariant> data=filterData.toMap();
    _roomType=data.value(identifiers.roomType).toString();
    _maximumUsersMax=data.value(identifiers.maximumUsersMax).toInt();
    _maximumUsersMin=data.value(identifiers.maximumUsersMin).toInt();
    _usersMax=data.value(identifiers.usersMax).toInt();
    _usersMin=data.value(identifiers.usersMin).toInt();
    _maximumActorsMax=data.value(identifiers.maximumActorsMax).toInt();
    _maximumActorsMin=data.value(identifiers.maximumActorsMin).toInt();
    _actorsMax=data.value(identifiers.actorsMax).toInt();
    _actorsMin=data.value(identifiers.actorsMin).toInt();
    _earliest=data.value(identifiers.earliest).toDateTime();
    _latest=data.value(identifiers.latest).toDateTime();
    _custom=data.value(identifiers.custom,true).toBool();
    _notCustom=data.value(identifiers.notCustom,true).toBool();
    _owner=data.value(identifiers.owner).toString();
    _empty=checkEmpty();
}

FKRoomDataFilter::FKRoomDataFilter(const QString& roomType, const qint32 maximumUsersMax, const qint32 maximumUsersMin, const qint32 usersMax, const qint32 usersMin, const qint32 maximumActorsMax, const qint32 maximumActorsMin, const qint32 actorsMax, const qint32 actorsMin, const QDateTime& earliest, const QDateTime& latest, const bool custom, const bool notCustom, const QString& owner):
        _roomType(roomType),_maximumUsersMax(maximumUsersMax),_maximumUsersMin(maximumUsersMin),_usersMax(usersMax),_usersMin(usersMin),_maximumActorsMax(maximumActorsMax),_maximumActorsMin(maximumActorsMin),_actorsMax(actorsMax),_actorsMin(actorsMin),_earliest(earliest),_latest(latest),_custom(custom),_notCustom(notCustom),_owner(owner){
    _empty=checkEmpty();
}

QVariant FKRoomDataFilter::toVariant() const{
    QVariant filterData;
    if(!_empty){
         QMap<QString,QVariant> data;
        if(!_roomType.isEmpty())data[identifiers.roomType]=_roomType;
        if(_maximumUsersMax>0)data[identifiers.maximumUsersMax]=_maximumUsersMax;
        if(_maximumUsersMin>0)data[identifiers.maximumUsersMin]=_maximumUsersMin;
        if(_usersMax>0)data[identifiers.usersMax]=_usersMax;
        if(_usersMin>0)data[identifiers.usersMin]=_usersMin;
        if(_maximumActorsMax>0)data[identifiers.maximumActorsMax]=_maximumActorsMax;
        if(_maximumActorsMin>0)data[identifiers.maximumActorsMin]=_maximumActorsMin;
        if(_actorsMax>0)data[identifiers.actorsMax]=_actorsMax;
        if(_actorsMin>0)data[identifiers.actorsMin]=_actorsMin;
        if(_earliest.isValid())data[identifiers.earliest]=_earliest;
        if(_latest.isValid())data[identifiers.latest]=_latest;
        if(!_custom)data[identifiers.custom]=_custom;
        if(!_notCustom)data[identifiers.notCustom]=_notCustom;
        if(!_owner.isEmpty())data[identifiers.owner]=_owner;
        filterData=data;
    }
    return filterData;
}

bool FKRoomDataFilter::checkEmpty() const{
    return _roomType.isEmpty() && _maximumUsersMax==0 && _maximumUsersMin==0 && _usersMax==0 && _usersMin==0 && _maximumActorsMax==0 && _maximumActorsMin==0 && _actorsMax==0 && _actorsMin==0 && !_earliest.isValid() && !_latest.isValid() && _custom && _notCustom && _owner.isEmpty();
}
