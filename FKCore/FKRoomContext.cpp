#include "FKRoomContext.h"

#include "FKRoom.h"
#include "FKUser.h"

#include "FKLogger.h"

/*!
\class FKRoomContext
\brief Context manages single room session with it's objects and UI
*/

FKRoomContext::FKRoomContext(QObject *parent) : QObject(parent){
    FK_CBEGIN
    FK_CEND
}

FKRoomContext::~FKRoomContext(){
    FK_DBEGIN
    FK_DEND
}

FKObject* FKRoomContext::roomObject() const{
    return _room;
}

FKObject*FKRoomContext::userObject() const{
    return _user;
}

QObject*FKRoomContext::rootEntity() const{
    return _rootEntity;
}

void FKRoomContext::setRoomObject(const qint32 objectId){
    _room=qobject_cast<FKRoom*>(getObject(objectId));
    emit roomObjectChanged();
}

void FKRoomContext::setRootEntity(QObject* entity){
    if(_rootEntity!=entity){
        _rootEntity=entity;
        emit rootEntityChanged();
    }
}

bool FKRoomContext::addClient(const FKRoomInviteData &invite){
    todo;
    return false;
}

bool FKRoomContext::removeClient(const FKRoomInviteData& invite){
    todo;
    return false;
}

void FKRoomContext::createObject(const QVariant& data){
    todo;
}

void FKRoomContext::deleteObject(const QVariant& data){
    todo;
}

void FKRoomContext::processAction(FKEventObject* ev){
    todo;
}

void FKRoomContext::processEvent(FKEventObject* ev){
    todo;
}

void FKRoomContext::setUser(const qint32 id){
    _user=qobject_cast<FKUser*>(getObject(id));
    emit userObjectChanged();
}

FKObject* FKRoomContext::genObject(const QString& className){
    qint32 id=_idGenerator.take();
    FKObject* obj=genObject(className,id,true);
    return obj;
}

/*!
 * \brief Get object by id. If target object is not managed by this manager, null would be reterned
 */

FKObject* FKRoomContext::getObject(const qint32 id){
    return _objects.value(id,nullptr);
}

void FKRoomContext::deleteObject(FKObject* obj){
    obj->executeEvent(FKIdentifiers::deinitObject);
    obj->executeEvent(FKIdentifiers::resetServant);
    _objects.remove(obj->getId());
    _idGenerator.back(obj->getId());
    //_objectPool.add(obj);
    obj->deleteLater();
}

void FKRoomContext::internalEvent(FKEventObject* event){
    todo;
    emit eventDispatched(event);
}

void FKRoomContext::internalAction(FKEventObject* action){
    todo;
    emit actionDispatched(action);
}

void FKRoomContext::shareObject(FKObject* obj, const QList<qint32>& recievers){
    todo;
}

void FKRoomContext::unshareObject(FKObject* obj, const QList<qint32>& recievers){
    todo;
}

/*!
 * \brief Create uninitialized object of given class, add to managed hash with given id and run initialization
 */

FKObject* FKRoomContext::genObject(const QString &className, const qint32 id,const bool createServant){
    //FKObject* obj=_objectPool.take(className);
    //if(!obj){
        FKObject* obj=FKObject::_factory.create(className);
        if(obj){
            obj->installObjectInfo();
            obj->setParent(this);
            obj->setRoomContext(this);
            if(createServant){
                obj->executeEvent(FKIdentifiers::createServant);
                obj->executeEvent(FKIdentifiers::resetServant);
            }
        }
    //}
    if(obj){
        obj->setId(id);
        _objects.insert(id,obj);
        obj->executeEvent(FKIdentifiers::initObject);
    }
    return obj;
}
