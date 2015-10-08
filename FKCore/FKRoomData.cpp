#include "FKRoomData.h"

#include <QVariant>

#include "FKLogger.h"
#include "FKRoomDataFilter.h"

#include "FKRoomSettings.h"

FKRoomData::FKRoomData(const qint32 server,const QString& newRoomType,const QString& ownerId,const QDateTime& roomCreationTime,const bool isCustom){
    _values.insert(FKRoomSettings::server,server);
    _values.insert(FKRoomSettings::roomType,newRoomType);
    _values.insert(FKRoomSettings::owner,ownerId);
    _values.insert(FKRoomSettings::creationTime,roomCreationTime);
    _values.insert(FKRoomSettings::isCustom,isCustom);
    _values.insert(FKRoomSettings::isCreatedByUser,!ownerId.isEmpty());
}

FKRoomData::FKRoomData(const QVariant &data){
    _values=data.value<QMap<qint32,QVariant>>();
}

FKRoomData::FKRoomData():FKRoomData(QVariant()){}

FKRoomData::FKRoomData(const FKRoomData& other)
    :_values(other._values)
{}

FKRoomData& FKRoomData::operator=(const FKRoomData& other){
    _values=other._values;
    return *this;
}

bool FKRoomData::isValid() const{
    return !_values.isEmpty();
}

QVariant FKRoomData::toVariant() const{
    return QVariant::fromValue(_values);
}

QVariant FKRoomData::createDelta(const qint32 propName, const QVariant &value){
    if(propName<=0 || !value.isValid()){
        return QVariant();
    }
    QPair<qint32,QVariant> data(propName,value);
    return QVariant::fromValue(data);
}

void FKRoomData::change(const QVariant& value){
    QPair<qint32,QVariant> val=value.value<QPair<qint32,QVariant>>();
    if(val.first>0 && val.second.isValid()){
        setValue(val.first,val.second);
    }else{
        FK_MLOG("Invalid room data change")
    }
}

bool FKRoomData::fit(const FKRoomDataFilter& filter)const{
    return filter._empty || (
               (filter._roomType.isEmpty()  || value(FKRoomSettings::roomType).toString()==filter._roomType)
            && (filter._maximumUsersMax==0  || value(FKRoomSettings::maximumUsers).toInt()<=filter._maximumUsersMax)
            && (                               value(FKRoomSettings::maximumUsers).toInt()>=filter._maximumUsersMin)
            && (filter._usersMax==0         || value(FKRoomSettings::totalUsers).toInt()<=filter._usersMax)
            && (                               value(FKRoomSettings::totalUsers).toInt()>=filter._usersMin)
            && (filter._maximumActorsMax==0 || value(FKRoomSettings::maximumActors).toInt()<=filter._maximumActorsMax)
            && (                               value(FKRoomSettings::maximumActors).toInt()>=filter._maximumActorsMin)
            && (filter._actorsMax==0        || value(FKRoomSettings::totalActors).toInt()<=filter._actorsMax)
            && (                               value(FKRoomSettings::totalActors).toInt()>=filter._actorsMin)
            && (!filter._latest.isValid()   || value(FKRoomSettings::creationTime).toDateTime()<=filter._latest)
            && (!filter._earliest.isValid() || value(FKRoomSettings::creationTime).toDateTime()>=filter._earliest)
            && (filter._owner.isEmpty()     || value(FKRoomSettings::owner).toString()==filter._owner)
            && ((filter._custom && value(FKRoomSettings::isCustom).toBool()) || (filter._notCustom && !value(FKRoomSettings::isCustom).toBool()))
                );
}

void FKRoomData::setValue(const qint32 propName, const QVariant &value){
    _values[propName]=value;
}

QVariant FKRoomData::value(const qint32 propName) const{
    return _values.value(propName);
}

QString FKRoomData::roomType() const{
    return value(FKRoomSettings::roomType).toString();
}


const FKRoomRequestData::Identifiers FKRoomRequestData::identifiers;

FKRoomRequestData::FKRoomRequestData(const QString& id, const QString& roomType, const QStringList& users, const bool custom):
    _id(id),_roomType(roomType),_users(users),_custom(custom){}

FKRoomRequestData::FKRoomRequestData(const QString& id, const QStringList& users):
    _id(id),_users(users),_custom(false){}

FKRoomRequestData::FKRoomRequestData(const QVariant& data){
    auto map=data.toMap();
    _id=map.value(identifiers.roomName).toString();
    _roomType=map.value(identifiers.roomType).toString();
    _users=map.value(identifiers.users).toStringList();
    _custom=map.value(identifiers.custom).toBool();
}

FKRoomRequestData::FKRoomRequestData(const FKRoomRequestData& other):
    _id(other._id),_roomType(other._roomType),_users(other._users),_custom(other._custom){}

QVariant FKRoomRequestData::toVariant() const{
    QMap<QString,QVariant> map;
    map[identifiers.roomName]=_id;
    map[identifiers.roomType]=_roomType;
    map[identifiers.users]=_users;
    if(_custom)map[identifiers.custom]=true;
    return map;
}
