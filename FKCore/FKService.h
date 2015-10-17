#ifndef FKSERVICE_H
#define FKSERVICE_H

#include "FKIdentifiers.h"

#include <QVariant>

struct FKCustomProperty{}; //these classes used as codegeneration marks for clang
struct FKCommonProperty{};
struct FKEventDeclaration{};
struct FKEventDeclarationForward{};
struct FKEventDeclarationReverse{};
struct FKActionDeclaration{};

class FKObject;
class FKEventObject;

namespace FK{

using commonProperty = FKCommonProperty;
using customProperty = FKCustomProperty;
using event = FKEventDeclaration;
using forwardComplexEvent = FKEventDeclarationForward;
using reverseComplexEvent = FKEventDeclarationReverse;
using action = FKActionDeclaration;
using read = QVariant;
using write = void;

#define SERVANT \
    forwardComplexEvent createServant; \
    forwardComplexEvent initServant; \
    reverseComplexEvent resetServant; \
    reverseComplexEvent deleteServant; \
    struct Servant; \
    Servant* servant=0

class FKBaseProperty{
public:
    virtual ~FKBaseProperty(){}
    virtual read clientValue()const{return value;}
    virtual read serverValue()const{return value;}
    virtual write clientValue(const QVariant& val){value=val;}
    virtual write serverValue(const QVariant& val){value=val;}
protected:
    QVariant value;
};

template <class ClassName>
class FKClassProperty:public FKBaseProperty{
    ClassName* object;
public:
    FKClassProperty(ClassName* owner):object(owner){}
};

template <class ClassName, qint32 id>
class FKProperty:public FKClassProperty<ClassName>{
    FKProperty(FKObject* owner):FKClassProperty<ClassName>(static_cast<ClassName>(owner)){}
    ~FKProperty()=0;
}; //codegenerator create this template specialization

struct FKBaseEvent{
    virtual ~FKBaseEvent(){}
    virtual void doEvent(FKEventObject* /*ev*/){}
};

template <class ClassName>
class FKClassEvent:public FKBaseEvent{
protected:
    FKClassEvent(FKObject* obj):object(static_cast<ClassName*>(obj)){}
    ClassName* object;
};

template <class ClassName, qint32 id>
class FKEvent:public FKClassEvent<ClassName>{
    //virtual void doEvent(FKEventObject* /*ev*/){}
    ~FKEvent()=0;
public:
    FKEvent(FKObject* obj):FKClassEvent<ClassName>(obj){}
}; //codegenerator create this template specialization

template <class ClassName, qint32 id>
using FKAction = FKEvent<ClassName,id>;

struct FKPropertyCreatorBase{
    virtual ~FKPropertyCreatorBase(){}
    virtual FKBaseProperty* create(FKObject* /*owner*/)=0;
};

template <class ClassName, qint32 id>
struct FKPropertyCreator:public FKPropertyCreatorBase{
    virtual FKBaseProperty* create(FKObject* owner)override{return new FKProperty<ClassName,id>(owner);}
};

struct FKEventExecutorBase{
    FKEventExecutorBase(const qint8 direction):direction(direction){}
    virtual ~FKEventExecutorBase(){}
    virtual void doEvent(FKObject* /*owner*/,FKEventObject* /*ev*/)=0;
    const qint8 direction;
};

template <class ClassName, qint32 id>
struct FKEventExecutor:public FKEventExecutorBase{
    FKEventExecutor(const qint8 direction):FKEventExecutorBase(direction){}
    virtual void doEvent(FKObject* owner,FKEventObject* ev)override{FKEvent<ClassName,id>(owner).doEvent(ev);}
};

class FKObjectInfoBase{
public:
    ~FKObjectInfoBase(){
        for(auto i=_commonProperties.begin();i!=_commonProperties.end();++i)delete i.value();
        for(auto i=_customProperties.begin();i!=_customProperties.end();++i)delete i.value();
        for(auto i=_events.begin();i!=_events.end();++i)delete i.value();
        for(auto i=_actions.begin();i!=_actions.end();++i)delete i.value();
    }
    inline const QMap<qint32,FKPropertyCreatorBase*>& commonProperties()const{return _commonProperties;}
    inline const QMap<qint32,FKPropertyCreatorBase*>& customProperties()const{return _customProperties;}
    inline const QMap<qint32,FKEventExecutorBase*>& events()const{return _events;}
    inline const QMap<qint32,FKEventExecutorBase*>& actions()const{return _actions;}
    inline const QMap<qint32,FKPropertyCreatorBase*>& inheritedCommonProperties()const{return _inheritedCommonProperties;}
    inline const QMap<qint32,FKPropertyCreatorBase*>& inheritedCustomProperties()const{return _inheritedCustomProperties;}
    inline const QMap<qint32,FKEventExecutorBase*>& inheritedEvents()const{return _inheritedEvents;}
    inline const QMap<qint32,FKEventExecutorBase*>& inheritedActions()const{return _inheritedActions;}
    inline const QList<qint32>& commonPropertyList()const{return _commonPropertyList;}
    inline const QList<qint32>& customPropertyList()const{return _customPropertyList;}
    inline const QList<qint32>& eventList()const{return _eventList;}
    inline const QList<qint32>& actionList()const{return _actionList;}
    inline const qint32& commonPropertyCount()const{return _commonPropertyCount;}
    inline const qint32& customPropertyCount()const{return _customPropertyCount;}
    inline const qint32& eventCount()const{return _eventCount;}
    inline const qint32& actionCount()const{return _actionCount;}
protected:
    template <class ClassName, qint32 id>
    void addCommon(){_commonProperties[id]=new FKPropertyCreator<ClassName,id>;}
    template <class ClassName, qint32 id>
    void addCustom(){_customProperties[id]=new FKPropertyCreator<ClassName,id>;}
    template <class ClassName, qint32 id>
    void addEvent(const qint8 direction){_events[id]=new FKEventExecutor<ClassName,id>(direction);}
    template <class ClassName, qint32 id>
    void addAction(){_actions[id]=new FKEventExecutor<ClassName,id>;}
private:
    QMap<qint32,FKPropertyCreatorBase*> _commonProperties;
    QMap<qint32,FKPropertyCreatorBase*> _customProperties;
    QMap<qint32,FKEventExecutorBase*> _events;
    QMap<qint32,FKEventExecutorBase*> _actions;
    QMap<qint32,FKPropertyCreatorBase*> _inheritedCommonProperties;
    QMap<qint32,FKPropertyCreatorBase*> _inheritedCustomProperties;
    QMap<qint32,FKEventExecutorBase*> _inheritedEvents;
    QMap<qint32,FKEventExecutorBase*> _inheritedActions;
    QList<qint32> _commonPropertyList;
    QList<qint32> _customPropertyList;
    QList<qint32> _eventList;
    QList<qint32> _actionList;
    qint32 _commonPropertyCount;
    qint32 _customPropertyCount;
    qint32 _eventCount;
    qint32 _actionCount;
    friend class FKObjectMetadata;
};

template <class ClassName>
struct FKObjectInfo:public FKObjectInfoBase{
    FKObjectInfo(){} //codegenerator adds info in constructor body
};

class FKObjectMetadata{
public:
    FKObjectMetadata(){}
    ~FKObjectMetadata(){
        for(auto i=classes.begin();i!=classes.end();++i)delete i.value();
    }
    inline FKObjectInfoBase* getObjectInfo(const QString& className)const{return classes.value(className,0);}
    template <class T>
    void addClass(){
        const QString className=T::staticMetaObject.className();
        if(classes.contains(className)){
            qWarning(QString("dublicate class registration attempt %1").arg(className).toLatin1());
            return;
        }
        FKObjectInfoBase* newClass=new FKObjectInfo<T>();
        newClass->_actionList=newClass->_actions.keys();
        newClass->_eventList=newClass->_events.keys();
        newClass->_commonPropertyList=newClass->_commonProperties.keys();
        newClass->_customPropertyList=newClass->_customProperties.keys();
        if(className!=QString("FKObject")){
            const QString parentClass=T::staticMetaObject.superClass()->className();
            if(!classes.contains(parentClass)){
                qWarning(QString("no parent class registered for %1").arg(className).toLatin1());
            }
            FKObjectInfoBase* parent=classes.value(parentClass,0);
            for(auto i=parent->_actions.constBegin();i!=parent->_actions.constEnd();++i){
                if(!newClass->_actionList.contains(i.key())){
                    newClass->_inheritedActions.insert(i.key(),i.value());
                    newClass->_actionList.append(i.key());
                }
            }
            for(auto i=parent->_events.constBegin();i!=parent->_events.constEnd();++i){
                if(!newClass->_eventList.contains(i.key())){
                    newClass->_inheritedEvents.insert(i.key(),i.value());
                    newClass->_eventList.append(i.key());
                }
            }
            for(auto i=parent->_commonProperties.constBegin();i!=parent->_commonProperties.constEnd();++i){
                if(!newClass->_commonPropertyList.contains(i.key())){
                    newClass->_inheritedCommonProperties.insert(i.key(),i.value());
                    newClass->_commonPropertyList.append(i.key());
                }
            }
            for(auto i=parent->_customProperties.constBegin();i!=parent->_customProperties.constEnd();++i){
                if(!newClass->_customPropertyList.contains(i.key())){
                    newClass->_inheritedCustomProperties.insert(i.key(),i.value());
                    newClass->_customPropertyList.append(i.key());
                }
            }
        }
        newClass->_customPropertyCount=newClass->_customPropertyList.size();
        newClass->_commonPropertyCount=newClass->_commonPropertyList.size();
        newClass->_eventCount=newClass->_eventList.size();
        newClass->_actionCount=newClass->_actionList.size();
        classes.insert(className,newClass);
    }
    template <class T>
    void removeClass(){
        const QString className=T::staticMetaObject.className();
        auto info=classes.find(className);
        if(info==classes.end()){
            qWarning(QString("unregistered class registration attempt %1").arg(className).toLatin1());
            return;
        }
        delete info.value();
        classes.erase(info);
    }
private:
    QMap<QString,FKObjectInfoBase*> classes;
};

//class FKObject:public QObject{
//    Q_OBJECT
//public:
//    ~FKObject(){
//        for(auto i=_properties.begin();i!=_properties.end();++i)delete i.value();
//    }
//};
}

#endif
