#include "FKRoomInviteData.h"

#include "FKLogger.h"

#include <QVariant>
#include <QMap>

/*!
\class FKRoomInviteData
\brief Used to send invite data from realm to server and client
*/

const FKRoomInviteData::Identifiers FKRoomInviteData::identifiers;

FKRoomInviteData::FKRoomInviteData():_port(-1),_isValid(false){}

FKRoomInviteData::FKRoomInviteData(const QString& client, const QString& password):
    _client(client),_password(password),_isValid(false){}

FKRoomInviteData::FKRoomInviteData(const QVariant& data):_port(-1),_isValid(true){
    QMap<QString,QVariant> dataMap=data.toMap();
    auto i=dataMap.constFind(identifiers.client);
    QStringList userList;
    if(i!=dataMap.constEnd()){
        _client=i.value().toString();
    }

    i=dataMap.constFind(identifiers.roomType);
    if(i!=dataMap.constEnd()){
        _roomType=i.value().toString();
    }

    i=dataMap.constFind(identifiers.users);
    if(i!=dataMap.constEnd()){
        userList=i.value().toStringList();
    }

    i=dataMap.constFind(identifiers.password);
    if(i!=dataMap.constEnd()){
        _password=i.value().toString();
        if(_password.isEmpty())_isValid=false;
    }

    i=dataMap.constFind(identifiers.address);
    if(i!=dataMap.constEnd()){
        _address=i.value().toString();
    }

    i=dataMap.constFind(identifiers.port);
    if(i!=dataMap.constEnd()){
        bool b;
        _port=i.value().toInt(&b);
        if(!b)_isValid=false;
    }
    if(_isValid){
        _isValid =  (!userList.isEmpty() &&
                     !_client.isEmpty()) ||
                    (!_roomType.isEmpty() &&
                     !_address.isEmpty() &&
                     _port>=0);
        if(_isValid){
            for(auto i=userList.constBegin();i!=userList.constEnd();++i){
                if(!tryAddUser(*i))return;
            }
        }
    }
}

FKRoomInviteData::FKRoomInviteData(const FKRoomInviteData& other):
    _client(other._client),_password(other._password),_users(other._users),_address(other._address),_port(other._port),_roomType(other._roomType){}

FKRoomInviteData::~FKRoomInviteData(){}

bool FKRoomInviteData::addUser(const QString& user){
    if(!user.isEmpty() && !_users.contains(user)){
        _users.append(user);
        _isValid=true;
        return true;
    }
    return false;
}

QVariant FKRoomInviteData::toClientInvite() const{
    if(_isValid){
        QMap<QString,QVariant> dataMap;
        dataMap.insert(identifiers.password,_password);
        dataMap.insert(identifiers.roomType,_roomType);
        dataMap.insert(identifiers.address,_address);
        dataMap.insert(identifiers.port,_port);
        return dataMap;
    }else{
        return QVariant();
    }
}

QVariant FKRoomInviteData::toServerInvite() const{
    if(_isValid){
        QMap<QString,QVariant> dataMap;
        dataMap.insert(identifiers.client,_client);
        dataMap.insert(identifiers.password,_password);
        dataMap.insert(identifiers.users,_users);
        return dataMap;
    }else{
        return QVariant();
    }
}

bool FKRoomInviteData::isValid() const{
    return _isValid;
}

QString FKRoomInviteData::client() const{
    return _client;
}

QStringList FKRoomInviteData::users() const{
    return _users;
}

QString FKRoomInviteData::password() const{
    return _password;
}

QString FKRoomInviteData::address() const{
    return _address;
}

qint32 FKRoomInviteData::port() const{
    return _port;
}

QString FKRoomInviteData::roomType() const{
    return _roomType;
}

void FKRoomInviteData::setAddress(const QString& ip){
    _address=ip;
}

void FKRoomInviteData::setPort(const qint32 port){
    _port=port;
}

void FKRoomInviteData::setRoomType(const QString& rt){
    _roomType=rt;
}

bool FKRoomInviteData::tryAddUser(const QString& user){
    if(user.isEmpty() || _users.contains(user)){
        _isValid=false;
        _users.clear();
        FK_MLOG("Invalid user id for room invite data found")
        return false;
    }
    _users.append(user);
    return true;
}

