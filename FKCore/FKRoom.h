#ifndef FKROOM_H
#define FKROOM_H

#include "FKObject.h"

class FKRoomData;

class FKCORESHARED_EXPORT FK_ABSTRACT_OBJECT(FKRoom,FKObject)
    Q_OBJECT
public:
    void setup(const FKRoomData& data);
};


#endif // FKROOM_H
