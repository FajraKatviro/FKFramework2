#ifndef FKROOM_H
#define FKROOM_H

#include "FKObject.h"

class FKRoomData;

class FK_OBJECT(FKRoom,FKObject)
public:
    void setup(const FKRoomData& data);
};


#endif // FKROOM_H
