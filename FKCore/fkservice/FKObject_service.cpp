#include "FKObject_service.h"


void FKEvent<FKObject, FKIdentifiers::createServant>::doEvent(FKEventObject*){
    if(object->servant)
        object->servant=new FKObject::Servant;
}
