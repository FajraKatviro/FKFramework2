#include "FKObject.h"

#include "FKObjectManager.h"
#include "FKDataBase.h"
#include "FKEvent.h"

#include "FKLogger.h"

/*!
 * \class FKObject
 * \inmodule FKObjects
 * \brief Это базовый класс, реализующий концепцию управления объектами
 *
 * FKFramework предлагает собственную концепцию управления объектами (КУО)
 * КУО включает следующие основопологающие принципы:
 * \list
 *  \li Реестр объектов. Каждый объект имеет уникальный номер и может быть обнаружен по тому же номеру как на стороне сервера, так и на стороне любого клиента, если он там существует. Нумерация начинается с 0. Неопределенный номер равен -1
 *  \li Единство объекта. Объект создается и удаляется только на стороне сервера.
 *  \li Видимость. Объекты могут следить за другими объектами. На стороне видящего клиента объект гарантировано существует. Объект, пропавший из видимости клиента, удаляется. Действия и события могут приходить только для видимых объектов.
 *  \li Иерархия видимости. Видимость не должна иметь цикличности и имеет 4 типа:
 *
 *      \list
 *
 *          \li минимальную - только общие свойства
 *          \li специальную - общие и уникальные свойства
 *          \li общую       - общие свойства и видимость всех далее видимых объектов
 *          \li полную      - все свойства и видимость всех далее видимых объектов
 *      \endlist
 *  \li Уникальные свойства. Объекты могут иметь свойства, видимые только определенному кругу клиентов. На стороне других клиентов значение свойства не обновляется.
 *  \li Общие свойства. Объекты могут иметь свойства, синхронизируемые на всех клиентах.
 *  \li События. Сервер может генерировать и отправлять для выполнения на стороне клиента события. Событие считается безусловно валидным и сразу передается к исполнению.
 *  \li Действия. Клиенты направляют на сервер запросы на выполнение действий. Система автоматически проверяет допустимость такого запроса и, в случае успеха, передает действие к исполению.
 *\endlist
 * FKObject реализует все необходимые инструменты для использования КУО.
 *
 * При наследовании класса необходимо использовать макросы FK_OBJECT(), FK_EVENTS(){}, FK_CONSTRUCTOR и FK_DESTRUCTOR
 */

/*!
 * \macro FK_OBJECT(...)
 *
 * \brief This macro must be placed in class definition instead of class specifier.
 *
 * Macro takes 2 arguments: className and inheritedClassName. The first '{' must be omitted.
\code
class FKOBJECTSSHARED_EXPORT FK_OBJECT(FKClient,FKObject)
    Q_OBJECT
public:

};
\endcode
 */

/*!
 * \macro FK_EVENTS(...)
 *
 * \brief This macro must be placed in *.cpp file for every class.
 *
 * Macro takes 1 argument: className.
 *
 * Instruction block {} after macro is required. Block determine class-specific events, actions and properties or can be empty.
\code
FK_EVENTS(FKCilent){
    events.insert(FKEventName::G_createObjects,&FKClient::E_createObjects);
    events.insert(FKEventName::G_deleteObjects,&FKClient::E_deleteObjects);
    actions.insert(FKEventName::A_setPassword,&FKClient::A_setPassword);
    props.append(FKEventName::C_password);
}
\endcode
 */

/*!
 * \macro FK_CONSTRUCTOR
 * \brief Use this macro at the beginning of constructor's implementation to correctly setup internal variables
 */

/*!
 * \macro FK_DESTRUCTOR
 * \brief Use this macro in the destructor to correct deletion of object
 */

/*!
 * \macro FK_NO_SERVANT
 * \brief Use this macro at the end of class body if there is no servant needed by class
 */

FKFactory<FKObject> FKObject::_factory;

/*!
 * \brief Constructs new object and register it for creator's object manager. Creator should be always passed for new objects in code. Null passed by system when constructing new objects for client-side. Use FK_CONSTRUCTOR macro when implementing new classes
 */

FKObject::FKObject():FKSystemObject(),servant(0){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Destructs object and deletes servant (if exists). You should use deleteObject() function instead of delete operator to correctly notify watching system. Use FK_DESTRUCTOR macro when implementing new classes
 */

FKObject::~FKObject(){
    FK_DBEGIN
    if(servant)delete servant;
    FK_DEND
}

/*!
 * \fn qint32 FKObject::getId() const
 * \brief Returns object's id. Id assigning at the moment of registration by object manager. Before that, id is invalid value of -1
 */

/*!
 * \brief Use this to safe deletion instead of deleteLater() or delete operator
 */

void FKObject::deleteObject(){
    if(servant){
        forbidControl();
        stopControl();
        stopTotalWatching();
        forbidTotalWatching();
    }
    emit deletedFKObject(this);
    _om->deleteObject(this);
}

/*!
 * \fn QString FKObject::getClassName()const
 * \brief Convinient function returns name of class
 */

/*!
 * \brief Returns object by id from object manager. If no such id registered, returns null
 */

FKObject* FKObject::getObject(const qint32 id)const{
    return _om->getObject(id);
}

/*!
 * \brief Request server to update at client-side all changed properties
 */

void FKObject::applyUpdates(){
    if(!servant){
        FK_MLOG("Unable apply updates: no servant")
        return;
    }
    FKEvent* event;
    QList<qint32> recievers;
    QSet<FKObject*> visors=totalVisors();
    foreach(FKObject* r,visors){
        recievers.append(r->getId());
    }
    QStringList props(commonPropertyList());
    auto u=servant->updateCommon.begin();
    auto p=props.constBegin();
    for(;u!=servant->updateCommon.end();++p, ++u){
        if(*u){
            event=FKEvent::makeEvent(getId(),*p,property(p->toLatin1()),recievers,true);
            _om->internalEvent(event);
            *u=false;
        }
    }
    props=customPropertyList();
    for(u=servant->updateCustom.begin(),p=props.constBegin();u!=servant->updateCustom.constEnd();++p, ++u){
        if(*u){
            refreshProperty(*p,customVisors(*p));
            *u=false;
        }
    }
}

/*!
 * \brief This function sets object's 'active' state. Actions from inactive actors would be ignored. Default value is true
 */

void FKObject::setActive(const bool t){
    if(!servant){
        FK_MLOG("Unable set object active: no servant")
        return;
    }
    servant->active=t;
}

/*!
 * \brief Server-side function. Starts watching for object and it's common properties
 */

void FKObject::startCommonWatching(FKObject* watched){
    if(watchingTest("start common watching",watched)){
        if(servant->watcheds.contains(watched))return;
        servant->watcheds.insert(watched);
        watched->servant->watchers.insert(this,QVector<bool>(watched->customPropertyCount(),false));
        watched->addCommonVisors(sharedVisors());
    }
}

/*!
 * \brief Server-side function. Starts watching for object and all it's custom and common properties
 */

void FKObject::startCustomWatching(FKObject* watched){
    if(watchingTest("start custom watching",watched)){
        startCommonWatching(watched);
        QStringList props(watched->customPropertyList());
        QSet<FKObject*> newVisors(sharedVisors());
        auto u=watched->servant->watchers[this].begin();
        auto p=props.constBegin();
        for(;p!=props.constEnd();++p, ++u){
            if(!*u){
                QSet<FKObject*> visors(newVisors);
                visors.subtract(watched->customVisors(*p));
                *u=true;
                if(!visors.isEmpty()){
                    watched->refreshProperty(*p,visors);
                }
            }
        }
    }
}

/*!
 * \brief Server-side function. Starts watching for object and it's common properties and custom property prop
 */


void FKObject::startCustomWatching(FKObject* watched, const QString& prop){
    if(watchingTest("start custom property watching",watched)){
        qint32 index=watched->customPropertyList().indexOf(prop);
        if(index==-1){
            FK_MLOGV("unable start watching unexisting custom property",prop)
            return;
        }
        startCommonWatching(watched);
        QVector<bool>::Iterator u=watched->servant->watchers[this].begin()+index;
        if(!*u){
            QSet<FKObject*> visors(sharedVisors());
            visors.subtract(watched->customVisors(prop));
            *u=true;
            if(!visors.isEmpty()){
                watched->refreshProperty(prop,visors);
            }
        }
    }
}

/*!
 * \brief Server-side function. Starts watching for object, it's common properties and all it's vision line
 */

void FKObject::startSharedWatching(FKObject* watched){
    if(watchingTest("start shared watching",watched)){
        startCommonWatching(watched);
        watched->addSharedWatcher(this);
    }
}

/*!
 * \brief Server-side function. Starts watching for object, all it's common and custom properties and all it's vision line
 */

void FKObject::startTotalWatching(FKObject* watched){
    if(watchingTest("start total watching",watched)){
        startCustomWatching(watched);
        watched->addSharedWatcher(this);
    }
}

/*!
 * \brief Server-side function. Stopd watching for object's vision line, all common and custom properties and object itself
 */

void FKObject::stopCommonWatching(FKObject* watched){
    if(watchingTest("stop common watching",watched)){
        if(!servant->watcheds.contains(watched))return;
        watched->resetCustomWatching(this);
        stopSharedWatching(watched);
        servant->watcheds.remove(watched);
        watched->servant->watchers.remove(this);
        watched->removeVisors(sharedVisors());
    }
}

/*!
 * \brief Server-side function. Stopd watching for object's all custom properties
 */

void FKObject::stopCustomWatching(FKObject* watched){
    if(watchingTest("stop custom watching",watched)){
        if(!servant->watcheds.contains(watched))return;
        watched->resetCustomWatching(this);
    }
}

/*!
 * \brief Server-side function. Stopd watching for object's custom property
 */

void FKObject::stopCustomWatching(FKObject* watched, const QString& prop){
    if(watchingTest("stop custom property watching",watched)){
        if(!servant->watcheds.contains(watched))return;
        qint32 index=watched->customPropertyList().indexOf(prop);
        if(index!=-1){
            watched->servant->watchers[this][index]=false;
        }else{
            FK_MLOGV("Unable stop watching unexisting property",prop)
        }
    }
}

/*!
 * \brief Server-side function. Stopd watching for object's vision line
 */

void FKObject::stopSharedWatching(FKObject* watched){
    if(watchingTest("stop shared watching",watched)){
        if(!servant->sharedWatcheds.contains(watched))return;
        servant->sharedWatcheds.remove(watched);
        watched->servant->sharedWatchers.remove(this);
        watched->removeTreeVisors(sharedVisors());
    }
}

/*!
 * \brief Same as stopCommonWatching(). If no watched object provided, applies for all watched objects
 */

void FKObject::stopTotalWatching(FKObject *watched){
    if(watched){
        stopCommonWatching(watched);
    }else{
        foreach(FKObject* obj,servant->watcheds)stopCommonWatching(obj);
    }
}

/*!
 * \brief Server-side function. Stops common watching by all object's watchers
 */

void FKObject::forbidTotalWatching(){
    foreach(FKObject* obj,servant->watchers.keys()){
        obj->stopCommonWatching(this);
    }
}

/*!
 * \brief Server-side function. Allow actor request actions for this object
 */

bool FKObject::allowControlBy(FKObject* actor,const QString &action){
    if(watchingTest("allow action",actor)){
        qint32 index=actionList().indexOf(action);
        if(index==-1){
            FK_MLOGV("Unable allow unexisting action",action)
            return false;
        }
        if(!servant->actors.contains(actor)){
            servant->actors.insert(actor,QVector<bool>(actionsCount(),false));
            actor->servant->controlled.insert(this);
        }
        servant->actors[actor][index]=true;
        return true;
    }
    return false;
}

/*!
 * \brief Server-side function. Allow actor request any actions for this object
 */

bool FKObject::allowControlBy(FKObject* actor){
    if(watchingTest("allow all actions",actor)){
        if(!servant->actors.contains(actor)){
            servant->actors.insert(actor,QVector<bool>(actionsCount(),true));
            actor->servant->controlled.insert(this);
        }else{
            servant->actors[actor].fill(true);
        }
        return true;
    }
    return false;
}

/*!
 * \brief Server-side function. Allow proccess actions, requested by object, recieved from given user side
 */

bool FKObject::allowEmitBy(FKObject* user){
    if(watchingTest("allow emit",user)){
        qint32 userId=user->getId();
        if(servant->emitters.contains(userId)){
            FK_MLOGV("Emitting already allowed by object",userId)
        }else{
            servant->emitters.append(userId);
            connect(user,SIGNAL(deletedFKObject(FKObject*)),SLOT(forbidEmitBy(FKObject*)));
        }
        return true;
    }
    return false;
}

/*!
 * \brief Server-side function. Forbid actor request any actions
 */

bool FKObject::stopControl(){
    if(!servant){
        FK_MLOGV("Unable stop control: no servant",getId())
        return false;
    }
    foreach(FKObject* obj,servant->controlled){
        if(!obj->forbidControlBy(this))return false;
    }
    return true;
}

/*!
 * \brief Server-side function. Forbid any actor request any action for this object
 */

bool FKObject::forbidControl(){
    if(!servant){
        FK_MLOGV("Unable forbid control: no servant",getId())
        return false;
    }
    foreach(FKObject* obj,servant->actors.keys()){
        if(!forbidControlBy(obj))return false;
    }
    return true;
}

/*!
 * \brief Server-side function. Forbid actor request actions for this object
 */

bool FKObject::forbidControlBy(FKObject* actor,const QString &action){
    if(watchingTest("forbid action",actor)){
        qint32 index=actionList().indexOf(action);
        if(index==-1){
            FK_MLOGV("Unable forbid unexisting action",action)
            return false;
        }
        if(!servant->actors.contains(actor)){
            FK_MLOGV("Acting forbidden at all from object",actor->getId())
            return true;
        }
        servant->actors[actor][index]=false;
        return true;
    }
    return false;
}

/*!
 * \brief Server-side function. Forbid actor request any actions for this object
 */

bool FKObject::forbidControlBy(FKObject* actor){
    if(watchingTest("forbid all actions",actor)){
        if(servant->actors.contains(actor)){
            servant->actors.remove(actor);
            actor->servant->controlled.remove(this);
        }
        return true;
    }
    return false;
}

/*!
 * \brief Server-side function. Forbid proccess actions, requested this by object, recieved from given user side
 */

bool FKObject::forbidEmitBy(FKObject* user){
    if(watchingTest("forbid emit",user)){
        qint32 userId=user->getId();
        if(servant->emitters.contains(userId)){
            servant->emitters.removeOne(userId);
            disconnect(user,SIGNAL(deletedFKObject(FKObject*)),this,SLOT(forbidEmitBy(FKObject*)));
        }else{
            FK_MLOGV("Unable forbid emit by user is not allowed before",user->getId())
        }
        return true;
    }
    return false;
}

void FKObject::servantInitialization(){
    servant->updateCommon.fill(false,commonPropertyCount());
    servant->updateCustom.fill(false,customPropertyCount());
}

void FKObject::servantDeinitialization(){
    servant->watchers.clear();
    servant->watcheds.clear();
    servant->sharedWatchers.clear();
    servant->sharedWatcheds.clear();
    servant->actors.clear();
    servant->controlled.clear();
    servant->emitters.clear();
    servant->sharedVisors.clear();
    servant->updateCommon.fill(false);
    servant->updateCustom.fill(false);
    servant->active=true;
}

void FKObject::installServant(){
    if(!servant){
        servant=new Servant();
        FKObject::servantInitialization();
    }
}

void FKObject::resetServant(){
    if(servant){
        FKObject::servantDeinitialization();
    }
}

void FKObject::logConstructor(){
    FK_CBEGIN
    FK_CEND
}

void FKObject::logDestructor(){
    FK_DBEGIN
    FK_DEND
}


/*!
 * \fn void FKObject::deletedFKObject(FKObject* obj)
 * \brief Signal emitted after purging all specific resources of FKObject before deleteLater() call
 */

/*!
 * \fn void FKObject::customInitialization()
 * \brief Execute object's class-specific initialization. This function declared by FK_OBJECT macro and must be implemented fot every subclass.
 */

/*!
 * \fn void FKObject::customDeinitialization()
 * \brief Execute object's class-specific deinitialization from destructor before servant deletion. This function declared by FK_OBJECT macro and must be implemented fot every subclass.
 */

/*!
 * \fn void FKObject::totalInitialization()
 * \brief Calls customInitialization() for all type hierarchy. Used by client-side object creation system. Implemented in FK_OBJECT macro and shouldn't be used in code.
 */

/*!
 * \brief Returns database from object manager for slow property read/write
 */

FKDataBase* FKObject::database() const{
    return _om->dataBase();
}

FKDBIndex FKObject::selfIndex()const{
    QStringList lst(QStringLiteral("FKObject"));
    lst.append(QString::number(getId()));
    return lst;
}

FKSystemObject *FKObject::clone() const{
    FK_MLOG("Warning! FKObject::clone() base implementation call. Seems it should be reimplemented")
    return 0;
}

bool FKObject::packObject(QDataStream &stream) const{
    Q_UNUSED(stream)
    FK_MLOG("Warning! FKObject::packObject() base implementation call. Seems it should be reimplemented")
    return true;
}

bool FKObject::loadObject(QDataStream &stream){
    Q_UNUSED(stream)
    FK_MLOG("Warning! FKObject::loadObject() base implementation call. Seems it should be reimplemented")
    return true;
}

/*!
 * \brief Writes numeric property value to database
 */

void FKObject::writeSlowPropertyNum(const QString& propertyName, const qreal value) const{
    _om->dataBase()->writeValue(FKDBValue(value),selfIndex()>>propertyName,false);
}

/*!
 * \brief Writes string property value to database
 */

void FKObject::writeSlowPropertyStr(const QString& propertyName, const QString& value) const{
    _om->dataBase()->writeValue(FKDBValue(value),selfIndex()>>propertyName,false);
}

/*!
 * \brief Writes index property value to database
 */

void FKObject::writeSlowPropertyRef(const QString& propertyName, const FKDBIndex& value) const{
    _om->dataBase()->writeValue(FKDBValue(value),selfIndex()>>propertyName,false);
}

/*!
 * \brief Returns numeric property value from database or 0 if no value found
 */

qreal FKObject::readSlowPropertyNum(const QString& propertyName) const{
    return _om->dataBase()->getValue(selfIndex()>>propertyName,false).number();
}

/*!
 * \brief Returns string property value from database or empty string if no value found
 */

QString FKObject::readSlowPropertyStr(const QString& propertyName) const{
    return _om->dataBase()->getValue(selfIndex()>>propertyName,false).string();
}

/*!
 * \brief Returns index property value from database or invalid index if no value found
 */

FKDBIndex FKObject::readSlowPropertyRef(const QString& propertyName) const{
    return _om->dataBase()->getValue(selfIndex()>>propertyName,false).index();
}

FKObject* FKObject::createObject(const QString& className) const{
    return _om->genObject(className);
}

/*!
 * \brief Server-side function. Send text message to reciever's visors. If no reciever provided, send to object's visors
 */

void FKObject::showMessage(const QString& msg, FKObject* reciever){
    if(!reciever)reciever=this;
    QList<qint32> recievers;
    foreach(FKObject* obj,reciever->totalVisors())recievers.append(obj->getId());
    _om->internalMessageRequest(msg,recievers);
}

/*!
 * \brief This is overloaded function
 */

void FKObject::doEvent(FKObject* target, const QString& subject, const QVariant& value, FKObject* reciever){
    if(!target){
        FK_MLOGV("Unable request event for null object",subject)
        return;
    }
    if(!reciever)reciever=target;
    doEvent(target->getId(),subject,value,reciever);
}

/*!
 * \brief This is overloaded function
 */

void FKObject::doEvent(FKObject* target, const QString& subject, FKObject* reciever){
    doEvent(target,subject,QVariant(),reciever);
}

/*!
 * \brief  Server-side function. Sends \i subject event for \i target with \i value to \i reciever's visors. If no reciever provided, send to target's visors instead.
 */

void FKObject::doEvent(const qint32 target, const QString& subject, const QVariant& value, FKObject* reciever){
    if(!reciever)reciever=getObject(target);
    QList<qint32> recievers;
    foreach(FKObject* obj,reciever->sharedVisors())recievers.append(obj->getId());
    if(!recievers.isEmpty()){
        FKEvent* event=FKEvent::makeEvent(target,subject,value,recievers,false);
        _om->internalEvent(event);
    }
}

/*!
 * \brief This is overloaded function
 */

void FKObject::doEvent(const qint32 target, const QString& subject, FKObject* reciever){
    doEvent(target,subject,QVariant(),reciever);
}

/*!
 * \brief Client-side function. Sends \i subject action for \i target object with \i value by this object from manager's root.
 */

void FKObject::doAction(const qint32 target, const QString& subject, const QVariant& value){
    FKEvent* action=FKEvent::makeAction(target,subject,value,_om->clientId(),getId());
    _om->internalAction(action);
}

/*!
 * \brief Server-side function. Marks given common ptoperty as changed, for future for update by applyUpdates()
 */

void FKObject::updateCommon(const QString& propertyName){
    if(servant){
        qint32 index=commonPropertyList().indexOf(propertyName);
        if(index!=-1){
            servant->updateCommon[index]=true;
        }
    }
}

/*!
 * \brief Server-side function. Marks given custom ptoperty as changed, for future for update by applyUpdates()
 */

void FKObject::updateCustom(const QString& propertyName){
    if(servant){
        qint32 index=customPropertyList().indexOf(propertyName);
        if(index!=-1){
            servant->updateCustom[index]=true;
        }
    }    
}

/*!
 * \fn qint32 commonPropertyCount()const
 * \brief Returns number of existing common properties
 */

/*!
 * \fn qint32 customPropertyCount()const
 * \brief Returns number of existing custom properties
 */

/*!
 * \fn qint32 actionsCount()const
 * \brief Returns number of existing actions
 */

/*!
 * \fn const QStringList eventList()const
 * \brief Returns list of existing events
 */

/*!
 * \fn const QStringList actionList()const
 * \brief Returns list of existing actions
 */

/*!
 * \fn const QStringList commonPropertyList()const
 * \brief Returns list of existing common properties
 */

/*!
 * \fn const QStringList customPropertyList()const
 * \brief Returns list of existing custom properties
 */

/*!
 * \fn void executeEvent(FKEvent* ev)
 * \brief This function created by FK_OBJECT macro for internal calls of events and shouldn't be used in code
 */

/*!
 * \fn void executeAction(FKEvent* ac)
 * \brief This function created by FK_OBJECT macro for internal calls of actions and shouldn't be used in code
 */

void FKObject::processFKAction(FKEvent* action){
    if(servant){
        qint32 index=actionList().indexOf(action->subject());
        if(index!=-1){   //если распознано действие
            FKObject* obj=getObject(action->emitter()); //распознать эмитента
            if(!obj){
                FK_MLOGV("Reject execute action with hidden emitter",getId())
            }else if(!obj->servant->active){
                FK_MLOGV("Actor inactive",action->emitter())
                return;
                //если эмитент активен
            }else if(action->client()==getId() || obj->servant->emitters.contains(action->client())){
                //если эмитент и есть клиент
                //или эмитент может действовать от лица клиента
                if(!obj->getId()==getId()){
                    //если эмитент - не сам объект
                    QMap<FKObject*,QVector<bool> >::ConstIterator i=servant->actors.constFind(obj);
                    if(i==servant->actors.constEnd() || !i.value().at(index)){
                        FK_MLOGV("Object is not under actors control",action->emitter())
                        return;
                        //и эмитент может управлять объектом,
                        //в том числе совершать указанное действие
                    }
                }
                //или если эмитент - сам объект
                executeAction(action);
            }else{
                FK_MLOGV("Emitting is not allowed by client",action->client())
            }
        }else{
            FK_MLOGV("Object has no requested action",action->subject())
        }
    }else{
        FK_MLOG("No servant to process action")
    }
}

void FKObject::processFKEvent(FKEvent* event){
    if(event->isPropertyNotifier()){
        if(commonPropertyList().contains(event->subject()) || customPropertyList().contains(event->subject())){
            setProperty(event->subject().toLatin1(),event->value());
        }else{
            FK_MLOGV("Object has no requested property",getClassName()+QString("::")+event->subject())
        }
    }else{
        if(eventList().contains(event->subject())){
            executeEvent(event);
        }else{
            FK_MLOGV("Object has no requested event",getClassName()+QString("::")+event->subject())
        }
    }
}

QSet<FKObject*> FKObject::customVisors(const QString &prop)const{
    QSet<FKObject*> v;
    qint32 index=customPropertyList().indexOf(prop);
    if(index!=-1){
        for(auto i=servant->watchers.constBegin();i!=servant->watchers.constEnd();++i){
            if(i.value().at(index))v.insert(i.key());
        }
    }
    return v;
}

QSet<FKObject*> FKObject::sharedVisors() const{
    return servant->sharedVisors;
}

QSet<FKObject*> FKObject::totalVisors() const{
    QSet<FKObject*> v;
    foreach(FKObject* obj,servant->watchers.keys()){
        v.unite(obj->sharedVisors());
    }
    return v;
}

QSet<FKObject*> FKObject::recalculatedSharedVisors() const{
    QSet<FKObject*> v;
    foreach(FKObject* obj,servant->sharedWatchers){
        v.unite(obj->sharedVisors());
    }
    return v;
}

void FKObject::addCommonVisors(QSet<FKObject*> newRecievers){
    newRecievers.subtract(totalVisors());
    if(!newRecievers.isEmpty()){
        QList<qint32> eventRecievers;
        foreach(FKObject* r,newRecievers){
            eventRecievers.append(r->getId());
        }
        initCommonProperties(eventRecievers);
    }
}

void FKObject::addSharedWatcher(FKObject* watcher){
    if(!servant->sharedWatchers.contains(watcher)){
        addSharedVisors(watcher->sharedVisors());
        servant->sharedWatchers.insert(watcher);
        watcher->servant->sharedWatcheds.insert(this);
    }
}

void FKObject::addSharedVisors(QSet<FKObject*> newRecievers){
    newRecievers.subtract(sharedVisors());
    if(!newRecievers.isEmpty()){
        servant->sharedVisors.unite(newRecievers);
        QList<qint32> visors;
        foreach(FKObject* obj,newRecievers){
            visors.append(obj->getId());
        }
        foreach(FKObject* obj,servant->watcheds){
            obj->initVisibleProperties(this,visors);
        }
        foreach(FKObject* obj,servant->sharedWatcheds){
            obj->addSharedVisors(newRecievers);
        }
    }
}

void FKObject::refreshProperty(const QString& propertyName, const QSet<FKObject*>& recievers){
    QList<qint32> recieversList;
    foreach(FKObject* r,recievers){
        recieversList.append(r->getId());
    }
    FKEvent* event=FKEvent::makeEvent(getId(),propertyName,property(propertyName.toLatin1()),recieversList,true);
    _om->internalEvent(event);
}

void FKObject::initVisibleProperties(FKObject* watcher, const QList<qint32>& recievers){
    initCommonProperties(recievers);
    FKEvent* event;
    QStringList props(customPropertyList());
    auto u=servant->watchers[watcher].begin();
    auto p=props.constBegin();
    for(;p!=props.constEnd();++p, ++u){
        if(*u){
            event=FKEvent::makeEvent(getId(),*p,property(p->toLatin1()),recievers,true);
            _om->internalEvent(event);
        }
    }
}

void FKObject::initCommonProperties(const QList<qint32>& recievers){
    _om->shareObject(this,recievers);
    FKEvent* event;
    QStringList props(commonPropertyList());
    for(auto p=props.constBegin();p!=props.constEnd();++p){
        event=FKEvent::makeEvent(getId(),*p,property(p->toLatin1()),recievers,true);
        _om->internalEvent(event);
    }
}

void FKObject::resetCustomWatching(FKObject* watcher){
    servant->watchers[watcher].fill(false);
}

void FKObject::removeVisors(QSet<FKObject*> oldRecievers){
    oldRecievers.subtract(totalVisors());
    if(!oldRecievers.isEmpty()){
        QList<qint32> eventRecievers;
        foreach(FKObject* r,oldRecievers){
            eventRecievers.append(r->getId());
        }
        _om->unshareObject(this,eventRecievers);
    }
}

void FKObject::removeTreeVisors(QSet<FKObject*> oldRecievers){
    oldRecievers.subtract(recalculatedSharedVisors());
    if(!oldRecievers.isEmpty()){
        servant->sharedVisors.subtract(oldRecievers);
        removeVisors(oldRecievers);
        foreach(FKObject* obj,servant->sharedWatcheds){
            obj->removeTreeVisors(oldRecievers);
        }
    }
}

bool FKObject::watchingTest(const char* t, FKObject* watched) const{
    if(!servant){
        FK_MLOG(QString("Unable %1: no servant").arg(QString(t)))
        return false;
    }
    if(!watched){
        FK_MLOG(QString("Unable %1: null argument").arg(QString(t)))
        return false;
    }
    if(watched==this){
        FK_MLOG(QString("Unable %1: self argument").arg(QString(t)))
        return false;
    }
    return true;
}
