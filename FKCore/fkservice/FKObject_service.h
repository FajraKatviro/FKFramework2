#ifndef FKOBJECT_SERVICE_H
#define FKOBJECT_SERVICE_H

#include "FKObject_identifiers.h"

class FKObject;
class FKEventObject;

namespace FK{

template<class T>
class FKClassEvent;

template <>
class FKEvent<FKObject,FKIdentifiers::createServant>:public FKClassEvent<FKObject>{
    void doEvent(FKEventObject* /*ev*/);
public:
    FKEvent(FKObject* obj):FKClassEvent<FKObject>(obj){}
};

template <>
class FKEvent<FKObject,FKIdentifiers::resetServant>:public FKClassEvent<FKObject>{
    void doEvent(FKEventObject* /*ev*/);
public:
    FKEvent(FKObject* obj):FKClassEvent<FKObject>(obj){}
};

}

#endif
