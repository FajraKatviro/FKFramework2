#include "FKObjectManager.h"

#include <QMutexLocker>

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

FKObjectManager::FKObjectManager(QObject* parent):QObject(parent),_mutex(QMutex::Recursive),_rootObject(0),_db(0){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Destructs manager and all managed objects
 */

FKObjectManager::~FKObjectManager(){
    foreach(FKObject* obj,_objects)obj->deleteObject();
}

/*!
 * \brief Get object by id. If target object is not managed by this manager, null would be reterned
 */

FKObject* FKObjectManager::getObject(const qint32 id){
    QMutexLocker m(&_mutex);
    return _objects.value(id,0);
}

/*!
 * \brief Add object to managed hash
 */

void FKObjectManager::addObject(FKObject* obj){
    qint32 id=obj->getId();
#ifdef FK_DEBUG
    if(id==-1){
        FK_MLOG("requested managing object has invalid id")
        return;
    }
    if(_objects.contains(id)){
        FK_MLOGV("requested managing object has already managed id",id)
        return;
    }
#endif
    _objects.insert(id,obj);
}

/*!
 * \brief Assign new id to object and add object to managed hash
 */

void FKObjectManager::regObject(FKObject* obj){
    QMutexLocker m(&_mutex);
    obj->setId(_idGenerator.take());
    addObject(obj);
}

/*!
 * \brief Create uninitialized object of given class, add to managed hash with given id and run initialization
 */

void FKObjectManager::genObject(const QString &className,const qint32 id){
    FKObject* obj=FKObject::create(className);
    if(obj){
        obj->setId(id);
        obj->setObjectManager(this);
        _mutex.lock();
        addObject(obj);
        _mutex.unlock();
        obj->totalInitialization();
    }
}

/*!
 * \brief Remove object from managed hash
 */

void FKObjectManager::freeObject(const qint32 id){
    QMutexLocker m(&_mutex);
    _objects.remove(id);
    _idGenerator.back(id);
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

/*!
 * \brief Notify to delete object at side of recievers. This function should be overriden, basic implementation does nothing
 */

void FKObjectManager::unshareObject(FKObject* obj,const QList<qint32>& recievers){
    Q_UNUSED(recievers)
    FK_MLOGV("Unshare object requested",obj->getId())
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

/*!
 * \fn void FKObjectManager::setRootObject(FKObject* obj)
 * \brief Set manager's root object
 */

/*!
 * \fn FKObject* FKObjectManager::rootObject()const
 * \brief Returns previously setted root object
 */
