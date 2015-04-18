#include "FKRoomData.h"

#include <QVariant>

#include "FKRoomDataFilter.h"

FKRoomData::Identifiers FKRoomData::identifiers=FKRoomData::identifiers();

FKRoomData::FKRoomData(const qint32 server,const QString& newRoomType,const QString& ownerId,const QDateTime& roomCreationTime,const bool isCustom):
        _roomType(newRoomType),_owner(ownerId),_creationTime(roomCreationTime),_custom(isCustom),
        _maximumUsers(0),_users(0),_maximumActors(0),_actors(0),_server(server),_ready(false){
}

FKRoomData::FKRoomData(const QVariant &data):_server(-1),_ready(false){
    const QMap<QString,QVariant> map=data.toMap();
    _roomType=map.value(identifiers.roomType).toString();
    _owner=map.value(identifiers.owner).toString();
    _creationTime=map.value(identifiers.creationTime).toDateTime();
    _custom=map.value(identifiers.custom).toBool();
    _maximumUsers=map.value(identifiers.maximumUsers).toInt();
    _users=map.value(identifiers.users).toInt();
    _maximumActors=map.value(identifiers.maximumActors).toInt();
    _actors=map.value(identifiers.actors).toInt();
}

FKRoomData::FKRoomData():FKRoomData(QVariant()){}

FKRoomData::FKRoomData(const FKRoomData& other)
    :_roomType(other._roomType)
    ,_owner(other._owner)
    ,_creationTime(other._creationTime)
    ,_custom(other._custom)
    ,_maximumUsers(other._maximumUsers)
    ,_users(other._users)
    ,_maximumActors(other._maximumActors)
    ,_actors(other._maximumActors)
    ,_server(other._server)
    ,_ready(other._ready)
{}

bool FKRoomData::isValid() const{
    return !_roomType.isEmpty();
}

QVariant FKRoomData::toVariant() const{
    QMap<QString,QVariant> data;
    data.insert(identifiers.roomType,_roomType);
    data.insert(identifiers.owner,_owner);
    data.insert(identifiers.creationTime,_creationTime);
    data.insert(identifiers.custom,_custom);
    data.insert(identifiers.maximumUsers,_maximumUsers);
    data.insert(identifiers.users,_users);
    data.insert(identifiers.maximumActors,_maximumActors);
    data.insert(identifiers.actors,_actors);
    return data;
}

QVariant FKRoomData::createDelta(const qint32 maxActorsChange, const qint32 actorsChange, const qint32 maxUsersChange, const qint32 usersChange){
    QMap<QString,QVariant> data;
    if(maxUsersChange)data[identifiers.maximumUsers]=maxUsersChange;
    if(usersChange)data[identifiers.users]=usersChange;
    if(maxActorsChange)data[identifiers.maximumActors]=maxActorsChange;
    if(actorsChange)data[identifiers.actors]=actorsChange;
    return data.isEmpty() ? QVariant() : data;
}

void FKRoomData::change(const QVariant& delta){
    QMap<QString,QVariant> map=delta.toMap();
    _maximumUsers+=map.value(identifiers.maximumUsers).toInt();
    _users+=map.value(identifiers.users).toInt();
    _maximumActors+=map.value(identifiers.maximumActors).toInt();
    _actors+=map.value(identifiers.actors).toInt();
}

bool FKRoomData::fit(const FKRoomDataFilter& filter)const{
    return _ready && (filter.empty || (
               (filter.roomType.isEmpty()  || _roomType==filter.roomType)
            && (filter.maximumUsersMax==0  || _maximumUsers<=filter.maximumUsersMax)
            && (                              _maximumUsers>=filter.maximumUsersMin)
            && (filter.usersMax==0         || _users<=filter.usersMax)
            && (                              _users>=filter.usersMin)
            && (filter.maximumActorsMax==0 || _maximumActors<=filter.maximumActorsMax)
            && (                              _maximumActors>=filter.maximumActorsMin)
            && (filter.actorsMax==0        || _actors<=filter.actorsMax)
            && (                              _actors>=filter.actorsMin)
            && (!filter.latest.isValid()   || _creationTime<=filter.latest)
            && (!filter.earliest.isValid() || _creationTime>=filter.earliest)
            && (filter.owner.isEmpty()     || _owner==filter.owner)
            && ((filter.custom && _custom) || (filter.notCustom && !_custom))
                          ));
}

void FKRoomData::changeUsers(const qint32 userCount){
    _users+=userCount;
}

FKRoomCreateData::Identifiers FKRoomCreateData::identifiers=FKRoomCreateData::Identifiers();

FKRoomCreateData::FKRoomCreateData(const QString& id, const QString& roomType):_id(id),_roomType(roomType){}

FKRoomCreateData::FKRoomCreateData(const QVariant& data){
    auto map=data.toMap();
    _id=map.value(identifiers.roomName);
    _roomType=map.value(identifiers.roomType);
}

FKRoomCreateData::FKRoomCreateData(const FKRoomCreateData& other):_id(other._id),_roomType(other._roomType){}

QVariant FKRoomCreateData::toVariant() const{
    QMap<QString,QVariant> map;
    map[identifiers.roomName]=_id;
    map[identifiers.roomType]=_roomType;
    return map;
}