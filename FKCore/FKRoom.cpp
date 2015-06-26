#include "FKRoom.h"

#include "FKUser.h"
#include "FKRoomInviteData.h"

struct FKRoom::Servant{
    QList<QObject*> users;
};

//void FKRoom::servantInitialization(){}

//void FKRoom::servantDeinitialization(){
//    QList<QObject*> emptyList;
//    servant->users.swap(emptyList);
//}

//FK_EVENTS(FKRoom){
//    common.append("o_usersSize");
//}

//void FKRoom::customInitialization(){
//    _usersSize=0;
//}

//void FKRoom::customDeinitialization(){
//    _roomSettings.clear();
//}

//----------------------------------------

void FKRoom::setup(const FKRoomData& data){
    //todo
    Q_UNUSED(data)
}


qint32 FKRoom::ro_usersSize() const{
    return servant->users.size();
}

void FKRoom::wo_usersSize(const qint32 size){
    _usersSize=size;
}

qint32 FKRoom::usersSize() const{
    return servant ? ro_usersSize() : _usersSize;
}


bool FKRoom::addUsers(const FKRoomInviteData& data){
    QList<qint32> userId;
    if(data.isValid()){
        qint32 usersInRoom=usersSize();
        qint32 usersToAdd=data.users().count();
        if(getSetting(FKRoomSettings::MaximumUsers).toInt()-usersInRoom>=usersToAdd){
            if(usersToAdd==1 || getSetting(FKRoomSettings::Neighbours).toBool()){
                foreach(auto userName,data.users()){
                    FKUser* user=createObject<FKUser>();
                    userId.append(user->getId());
                }
                //todo: keep user objects somewhere
                emit roomDataChanged(0,0,0,usersToAdd);
                //updateCommon("o_usersSize");
                applyUpdates();
                return true;
            }
        }
    }
    return false;
}
