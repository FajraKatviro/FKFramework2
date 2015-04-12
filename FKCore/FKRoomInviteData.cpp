#include "FKRoomInviteData.h"

#include "FKLogger.h"

#include <QVariant>
#include <QMap>

FKRoomInviteData::Identifiers FKRoomInviteData::identifiers=FKRoomInviteData::Identifiers();

FKRoomInviteData::FKRoomInviteData():_port(-1),_isValid(false){}

FKRoomInviteData::FKRoomInviteData(const QString& client):
    _client(client),_port(-1),_isValid(false){}

FKRoomInviteData::FKRoomInviteData(const QString& client, const QString& address, const qint32 port):
    _client(client),_address(address),_port(port),_isValid(false){}

FKRoomInviteData::FKRoomInviteData(const QVariant& data):_port(-1),_isValid(true){
    QMap<QString,QVariant> dataMap=data.toMap();
    auto i=dataMap.constFind(identifiers.client);
    QStringList userList, passwordList;
    if(i!=dataMap.constEnd()){
        _client=i.value().toString();

        i=dataMap.constFind(identifiers.users);
        if(i!=dataMap.constEnd()){
            userList=i.value().toStringList();
        }

        i=dataMap.constFind(identifiers.passwords);
        if(i!=dataMap.constEnd()){
            passwordList=i.value().toStringList();
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
    }else{
        _isValid=false;
    }
    if(_isValid){
        _isValid =  !_client.isEmpty() &&
                    !userList.isEmpty() &&
                    (passwordList.isEmpty() || passwordList.size()==userList.size()) &&
                    (_address.isEmpty() || _port>=0);
        if(_isValid){
            if(passwordList.isEmpty()){
                for(auto i=userList.constBegin();i!=userList.constEnd();++i){
                    if(!tryAddUser(*i,QString()))return;
                }
            }else{
                for(auto i=userList.constBegin(),p=passwordList.constBegin();i!=userList.constEnd();++i,++p){
                    if(!tryAddUser(*i,*p))return;
                }
            }
        }
    }
}

FKRoomInviteData::FKRoomInviteData(const FKRoomInviteData& other):
    _client(other._client),_passwords(other._passwords),_address(other._address),_port(other._port){}

FKRoomInviteData::~FKRoomInviteData(){}

bool FKRoomInviteData::addUser(const QString& user){
    return addUser(user,QString());
}

bool FKRoomInviteData::addUser(const QString& user, const QString& password){
    if(!user.isEmpty() && !_passwords.contains(user)){
        _passwords.insert(user,password);
        return true;
    }
    return false;
}

QVariant FKRoomInviteData::toVariant() const{
    if(_isValid){
        QMap<QString,QVariant> dataMap;

        bool hasPasswords=false, hasAddress=!_address.isEmpty();

        QStringList passwords=_passwords.values();
        for(auto i=passwords.constBegin();i!=passwords.constEnd();++i){
            if(!(*i).isEmpty()){
                hasPasswords=true;
                break;
            }
        }

        dataMap.insert(identifiers.client,_client);
        dataMap.insert(identifiers.users,_passwords.keys());
        if(hasPasswords)
            dataMap.insert(identifiers.passwords,passwords);
        if(hasAddress){
            dataMap.insert(identifiers.address,_address);
            dataMap.insert(identifiers.port,_port);
        }

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
    return _passwords.keys();
}

QMap<QString, QString> FKRoomInviteData::passwords() const{
    return _passwords;
}

QString FKRoomInviteData::address() const{
    return _address;
}

qint32 FKRoomInviteData::port() const{
    return _port;
}

void FKRoomInviteData::setAddress(const QString& ip){
    _address=ip;
}

bool FKRoomInviteData::tryAddUser(const QString& user,const QString& password){
    if(user.isEmpty() || _passwords.contains(user)){
        _isValid=false;
        _passwords.clear();
        FK_MLOG("Invalid user id for room invite data found")
        return false;
    }
    _passwords.insert(user,password);
    return true;
}

