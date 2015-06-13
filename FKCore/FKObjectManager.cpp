#include "FKObjectManager.h"

#include "FKLogger.h"
#include "FKEvent.h"
#include "FKMessage.h"
#include "FKObject.h"

/*!
 * \class FKObjectManager
 * \brief Abstract class provides access to FKObjects via id. Objects saving in hash on creating and removes automatically on deleting
 */

/*!
 * \brief Constructs object with given parent
 */

FKObjectManager::FKObjectManager(QObject* parent):QObject(parent),_objectPool(20),_db(0),_roomModule(0){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Destructs manager and all managed objects
 */

FKObjectManager::~FKObjectManager(){

}

FKObject* FKObjectManager::genObject(const QString& className){
    qint32 id=_idGenerator.take();
    FKObject* obj=genObject(className,id);
    obj->servantInitialization();
    return obj;
}

/*!
 * \brief Get object by id. If target object is not managed by this manager, null would be reterned
 */

FKObject* FKObjectManager::getObject(const qint32 id){
    return _objects.value(id,0);
}

void FKObjectManager::deleteObject(FKObject* obj){
    obj->totalDeinitialization();
    obj->resetServant();
    _objects.remove(obj->getId());
    _idGenerator.back(obj->getId());
    _objectPool.add(obj);
}

/*!
 * \brief Create uninitialized object of given class, add to managed hash with given id and run initialization
 */

FKObject* FKObjectManager::genObject(const QString &className,const qint32 id){
    FKObject* obj=_objectPool.take(className);
    if(!obj)obj=FKObject::_factory.create(className);
    if(obj){
        obj->setParent(this);
        obj->setId(id);
        obj->setObjectManager(this);
        _objects.insert(id,obj);
        obj->totalInitialization();
    }
    return obj;
}

/*!
 * \brief Process event created at the same (client or server) side
 */

void FKObjectManager::internalEvent(FKEvent* event){
    event->deleteLater();
}

/*!
 * \brief Process event created at the opposite (client or server) side
 */

void FKObjectManager::externalEvent(FKEvent* event){
    event->deleteLater();
}

/*!
 * \brief Process action created at the same (client or server) side
 */

void FKObjectManager::internalAction(FKEvent* action){
    action->deleteLater();
}

/*!
 * \brief Process action created at the opposite (client or server) side
 */

void FKObjectManager::externalAction(FKEvent* action){
    action->deleteLater();
}

/*!
 * \brief Process text message created at the same (client or server) side
 */

void FKObjectManager::internalMessageRequest(const QString& msg, const QList<qint32>& recievers){
    Q_UNUSED(msg) Q_UNUSED(recievers)
}

/*!
 * \brief Process text message created at the opposite (client or server) side
 */

void FKObjectManager::externalMessageRequest(FKMessage* message, const qint32 sender){
    Q_UNUSED(sender)
    message->deleteLater();
}

/*!
 * \brief Notify to create object at side of recievers. This function should be overriden, basic implementation does nothing
 */

void FKObjectManager::shareObject(FKObject* obj, const QList<qint32>& recievers){
    Q_UNUSED(recievers)
    FK_MLOGV("Share object requested",obj->getId())
}

void FKObjectManager::createObject(const QVariant& data){
    FK_MLOGV("Create object request recieved",data)
}

/*!
 * \brief Notify to delete object at side of recievers. This function should be overriden, basic implementation does nothing
 */

void FKObjectManager::unshareObject(FKObject* obj,const QList<qint32>& recievers){
    Q_UNUSED(recievers)
    FK_MLOGV("Unshare object requested",obj->getId())
}

void FKObjectManager::deleteObject(const QVariant& data){
    FK_MLOGV("Delete object request recieved",data)
}

/*!
 * \fn qint32 FKObjectManager::clientId()const
 * \brief Returns id of object that refers to this manager root
 */

/*!
 * \fn void setDataBase(FKDataBase* db)
 * \brief Set database for operating with object's slow properties
 */

/*!
 * \fn FKDataBase* dataBase()const
 * \brief Returns previousli setted database
 */
