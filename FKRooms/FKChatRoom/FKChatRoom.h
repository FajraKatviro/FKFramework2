#ifndef FKCHATROOM_H
#define FKCHATROOM_H

#include "fkchatroom_global.h"
#include "FKRoomComponents.h"

class FKCHATROOMSHARED_EXPORT FK_OBJECT(FKChatRoom,FKRoom)
    Q_OBJECT
public:

};

FK_PACKAGE_MANAGER(FKChatRoomPackageManager){

}

FK_ROOM_MODULE(FKChatRoom,1.0.0,FKChatRoomPackageManager)

#endif // FKCHATROOM_H
