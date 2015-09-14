#include "FKRoomData.h"

#include <QVariant>

#include "FKRoomDataFilter.h"

const FKRoomData::Identifiers FKRoomData::identifiers;

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

FKRoomData& FKRoomData::operator=(const FKRoomData& other){
    _roomType=other._roomType;
    _owner=other._owner;
    _creationTime=other._creationTime;
    _custom=other._custom;
    _maximumUsers=other._maximumUsers;
    _users=other._users;
    _maximumActors=other._maximumActors;
    _actors=other._maximumActors;
    _server=other._server;
    _ready=other._ready;
    return *this;
}

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

QVariant FKRoomData::toCreationRequest() const{
    QMap<QString,QVariant> data;
    data.insert(identifiers.roomType,_roomType);
    data.insert(identifiers.owner,_owner);
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
    return _ready && (filter._empty || (
               (filter._roomType.isEmpty()  || _roomType==filter._roomType)
            && (filter._maximumUsersMax==0  || _maximumUsers<=filter._maximumUsersMax)
            && (                              _maximumUsers>=filter._maximumUsersMin)
            && (filter._usersMax==0         || _users<=filter._usersMax)
            && (                              _users>=filter._usersMin)
            && (filter._maximumActorsMax==0 || _maximumActors<=filter._maximumActorsMax)
            && (                              _maximumActors>=filter._maximumActorsMin)
            && (filter._actorsMax==0        || _actors<=filter._actorsMax)
            && (                              _actors>=filter._actorsMin)
            && (!filter._latest.isValid()   || _creationTime<=filter._latest)
            && (!filter._earliest.isValid() || _creationTime>=filter._earliest)
            && (filter._owner.isEmpty()     || _owner==filter._owner)
            && ((filter._custom && _custom) || (filter._notCustom && !_custom))
                          ));
}

void FKRoomData::changeUsers(const qint32 userCount){
    _users+=userCount;
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
