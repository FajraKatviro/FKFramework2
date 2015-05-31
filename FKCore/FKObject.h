#ifndef FKOBJECT_H
#define FKOBJECT_H

#include "FKSystemObject.h"

#include "FKObjectFactory.h"
#include "FKEvent.h"

#include <QVariant>
#include <QSet>
#include <QVector>

class FKObjectManager;
class FKDataBase;
class FKDBIndex;

#define FK_ABSTRACT_OBJECT(derivedClass,parentClass) \
FK_OBJECT_COMMON(derivedClass,parentClass) \

#define FK_OBJECT(derivedClass,parentClass) \
FK_OBJECT_COMMON(derivedClass,parentClass) \
friend class FKFactoryObjectCreator<derivedClass>;

#define FK_OBJECT_COMMON(derivedClass,parentClass) \
derivedClass : public parentClass{ \
    typedef void (derivedClass::*fkfPtr)(FKEvent*); \
    static class Events:public FKObject::Events{ \
        void initEvents(); \
        QMap<QString,derivedClass::fkfPtr> events; \
        QMap<QString,derivedClass::fkfPtr> actions; \
    public: \
        Events(){initEvents();} \
        QMap<QString,derivedClass::fkfPtr> eventList()const{return events;} \
        QMap<QString,derivedClass::fkfPtr> actionList()const{return actions;} \
    } _eventKeeper;\
    struct Servant;\
    Servant* servant;\
protected:\
    derivedClass():servant(0){logConstructor();} \
    ~derivedClass(){deleteServant(); logDestructor();}\
    virtual void customInitialization()override;\
    virtual void customDeinitialization()override;\
    virtual void totalInitialization()override{\
        parentClass::totalInitialization();\
        derivedClass::customInitialization();\
    }\
    virtual void totalDeinitialization()override{\
        derivedClass::customDeinitialization();\
        parentClass::customDeinitialization();\
    }\
    virtual void servantInitialization()override;\
    virtual void servantDeinitialization()override;\
    virtual void createServant()override;\
    virtual void deleteServant()override;\
    virtual void installServant()override{\
        if(!servant){\
            parentClass::installServant();\
            createServant();\
            derivedClass::servantInitialization();\
        }\
    }\
    virtual void resetServant(){\
        if(servant){\
            derivedClass::servantDeinitialization();\
            parentClass::resetServant();\
        }\
    }\
    virtual void executeEvent(FKEvent* ev)override{\
        derivedClass::fkfPtr p=_eventKeeper.eventList().value(ev->subject(),0);\
        if(p)(this->*p)(ev); else parentClass::executeEvent(ev);\
    }\
    virtual void executeAction(FKEvent* ac)override{\
        derivedClass::fkfPtr p=_eventKeeper.actionList().value(ac->subject(),0);\
        if(p)(this->*p)(ac); else parentClass::executeAction(ac);\
    }\
    virtual qint32 commonPropertyCount()const override{\
        static qint32 count=commonPropertyList().count();\
        return count;}\
    virtual qint32 customPropertyCount()const override{\
        static qint32 count=customPropertyList().count();\
        return count;}\
    virtual qint32 actionsCount()const override{\
        static qint32 count=actionList().count();\
        return count;}\
public:\
    virtual const QStringList eventList()const override{\
        static const QStringList p(_eventKeeper.eventList().keys()<<parentClass::eventList());\
        return p;} \
    virtual const QStringList actionList()const override{ \
        static const QStringList p(_eventKeeper.actionList().keys()<<parentClass::actionList()); \
        return p;} \
    virtual const QStringList commonPropertyList()const override{ \
        static const QStringList p(_eventKeeper.commonPropertyList()<<parentClass::commonPropertyList()); \
        return p;} \
    virtual const QStringList customPropertyList()const override{ \
        static const QStringList p(_eventKeeper.customPropertyList()<<parentClass::customPropertyList()); \
        return p;} \
private:

#define FK_EVENTS(className) \
className::Events className::_eventKeeper=className::Events(); \
void className::createServant(){servant=new Servant();} \
void className::deleteServant(){delete servant;} \
void className::Events::initEvents()

#define FK_NO_SERVANT(className) \
struct className::Servant{};\
void className::servantInitialization(){}\
void className::servantDeinitialization(){}\


class FKCORESHARED_EXPORT FKObject:public FKSystemObject{
    Q_OBJECT
public:
    FKObject();
    ~FKObject();
    FK_OBJECT_FACTORY
    qint32 getId()const{return _id;}
    QString getClassName()const{return metaObject()->className();}

    FKObject* getObject(const qint32 id)const;

    void applyUpdates();
    void setActive(const bool t);

    void startCommonWatching(FKObject* watched);
    void startCustomWatching(FKObject* watched);
    void startCustomWatching(FKObject* watched,const QString &prop);
    void startSharedWatching(FKObject* watched);
    void startTotalWatching(FKObject* watched);

    void stopCommonWatching(FKObject* watched);
    void stopCustomWatching(FKObject* watched);
    void stopCustomWatching(FKObject* watched,const QString &prop);
    void stopSharedWatching(FKObject* watched);
    void stopTotalWatching(FKObject* watched=0);
    void forbidTotalWatching();

    bool allowControlBy(FKObject* actor,const QString &action);
    bool allowControlBy(FKObject* actor);
    bool allowEmitBy(FKObject* user);
    bool stopControl();
    bool forbidControl();
    bool forbidControlBy(FKObject* actor,const QString &action);
    bool forbidControlBy(FKObject* actor);
public slots:
    bool forbidEmitBy(FKObject* user);
signals:
    void deletedFKObject(FKObject* obj);
protected:
//This section used by FK_OBJECT macro to properly load inherited objects
    virtual void customInitialization(){}   //initialize inheritence level, must be implemented
    virtual void customDeinitialization(){}
    virtual void totalInitialization(){}    //initialize object
    virtual void totalDeinitialization(){}

    virtual void servantInitialization();   //initialize servant inheritence level
    virtual void servantDeinitialization();
    virtual void installServant();          //create all level servants and initialize them
    virtual void resetServant();
    virtual void createServant(){}          //this function creates servant object. This is for FK_OBJECT macro to handle C++ limitations
    virtual void deleteServant(){}

    void logConstructor();
    void logDestructor();

//This is reimplemented functions (FKSystemObject)
//    bool packObject(QDataStream &stream)const;
//    bool loadObject(QDataStream &stream);

//This is DB access functions
    FKDataBase* database()const;
    void writeSlowPropertyNum(const QString& propertyName,const qreal value)const;
    void writeSlowPropertyStr(const QString& propertyName,const QString& value)const;
    void writeSlowPropertyRef(const QString& propertyName,const FKDBIndex& value)const;
    qreal readSlowPropertyNum(const QString& propertyName)const;
    QString readSlowPropertyStr(const QString& propertyName)const;
    FKDBIndex readSlowPropertyRef(const QString& propertyName)const;

//General operational requests
    template<class D> D* createObject()const{return static_cast<D*>(createObject(D::staticMetaObject.className()));}
    FKObject* createObject(const QString& className)const;
    void deleteObject();
    void showMessage(const QString& msg,FKObject* reciever=0);
    void doEvent(FKObject* target,const QString& subject,const QVariant& value=QVariant(),FKObject* reciever=0);
    void doEvent(FKObject* target, const QString& subject, FKObject* reciever);
    void doEvent(const qint32 target,const QString& subject,const QVariant& value=QVariant(),FKObject* reciever=0);
    void doEvent(const qint32 target, const QString& subject, FKObject* reciever);
    void doAction(const qint32 target,const QString& subject,const QVariant& value=QVariant());
    void updateCommon(const QString& propertyName);
    void updateCustom(const QString& propertyName);

//This section used by property and watching systems
    virtual qint32 commonPropertyCount()const{return 0;}
    virtual qint32 customPropertyCount()const{return 0;}
    virtual qint32 actionsCount()const{return 0;}
    virtual const QStringList eventList()const{return QStringList();}
    virtual const QStringList actionList()const{return QStringList();}
    virtual const QStringList commonPropertyList()const{return QStringList();}
    virtual const QStringList customPropertyList()const{return QStringList();}
    virtual void executeEvent(FKEvent* ev){Q_UNUSED(ev)}
    virtual void executeAction(FKEvent* ac){Q_UNUSED(ac)}

//This struct used by event system to hold event lists
    class Events{
        virtual void initEvents(){}
    protected:
        QStringList common;
        QStringList custom;
    public:
        Events(){}
        virtual ~Events(){}
        QStringList commonPropertyList()const{return common;}
        QStringList customPropertyList()const{return custom;}
    };
private:
//This section used by object manager
    void setId(const qint32 id){_id=id;}
    void setObjectManager(FKObjectManager* om){_om=om;}
    void processFKAction(FKEvent* action);
    void processFKEvent(FKEvent* event);
    friend class FKObjectManager;

//This section used by watching system
    QSet<FKObject*> customVisors(const QString &prop)const; //return visors for property. Used when updating custom property
    QSet<FKObject*> sharedVisors()const; //return visors of object's view; for user return itself. Used for vision changing. Returns cached set
    QSet<FKObject*> totalVisors()const; //return visors of object's view and of object itself. Used when updating common property. Calculates dynamically
    QSet<FKObject*> recalculatedSharedVisors()const;

    void addCommonVisors(QSet<FKObject*> newRecievers); //add newRecievers to shared watchers. Used when changing common vision
    void addSharedWatcher(FKObject* watcher);
    void addSharedVisors(QSet<FKObject*> newRecievers);
    void refreshProperty(const QString& propertyName,const QSet<FKObject*>& recievers); //send property to recievers. Used when changing custom vision or values
    void initVisibleProperties(FKObject* watcher, const QList<qint32>& recievers); //send all properties visible for watcher to recievers. Used when changing shared vision
    void initCommonProperties(const QList<qint32>& recievers);

    void resetCustomWatching(FKObject* watcher);
    void removeVisors(QSet<FKObject*> oldRecievers);
    void removeTreeVisors(QSet<FKObject*> oldRecievers);

    bool watchingTest(const char* t, FKObject* watched)const;

//database access functions
    FKDBIndex selfIndex()const;

//Servants are used within client-server dividing concept
    struct Servant{
        Servant():active(true){}
        QMap<FKObject*,QVector<bool> > watchers; //used by startCommonWatching() and startCustomWatching()
        QSet<FKObject*> watcheds; //used by startCommonWatching()
        QSet<FKObject*> sharedWatchers;
        QSet<FKObject*> sharedWatcheds; //used by startSharedWatching()
        QMap<FKObject*,QVector<bool> > actors; //used by processAction()
        QSet<FKObject*> controlled;
        QList<qint32> emitters; //used by action handler to validate action source. Usually short list
        QSet<FKObject*> sharedVisors; //used by doEvent(), updateCommon(), etc.
        QVector<bool> updateCommon; //used by updateCommon()
        QVector<bool> updateCustom; //used by updateCustom()
        bool active; //used by setActive() and processAction()
    };
    Servant* servant;

    qint32 _id;
    FKObjectManager* _om;
    static FKObjectFactory _factory;

    virtual FKSystemObject* clone()const override;
    virtual bool packObject(QDataStream& stream)const override;
    virtual bool loadObject(QDataStream& stream)override;
};

#endif // FKOBJECT_H
