#ifndef FKOBJECT_H
#define FKOBJECT_H

#include "FKSystemObject.h"

#include "FKService.h"
#include "FKFactory.h"
#include "FKEventObject.h"

#include <QVariant>
#include <QSet>
#include <QVector>
#include <QReadWriteLock>

class FKObjectManager;
class FKDataBase;
struct FKDBIndex;

using namespace FK;

class FKCORESHARED_EXPORT FKObject:public FKSystemObject{
    Q_OBJECT
public:
    FKObject();
    ~FKObject();
    SERVANT;

    FK_OBJECT_REGISTRATOR(Constructing)
    template<class D>
    static void allowConstructing(){
        _factory.addClass<D>();
        allowService<D>();
    }
    template<class D>
    static void forbidConstructing(){
        _factory.removeClass<D>();
        forbidService<D>();
    }

    FK_OBJECT_REGISTRATOR(Service)
    template<class D>
    static void allowService(){
        QWriteLocker lock(&objectMetadataLocker);
        objectMetadata.addClass<D>();
    }
    template<class D>
    static void forbidService(){
        QWriteLocker lock(&objectMetadataLocker);
        objectMetadata.removeClass<D>();
    }

    qint32 getId()const{return _id;}
    QString getClassName()const{return metaObject()->className();}

    FKObject* getObject(const qint32 id)const;

    void applyUpdates();
    void setActive(const bool t);

    void startCommonWatching(FKObject* watched);
    void startCustomWatching(FKObject* watched);
    void startCustomWatching(FKObject* watched, const qint32 prop);
    void startSharedWatching(FKObject* watched);
    void startTotalWatching(FKObject* watched);

    void stopCommonWatching(FKObject* watched);
    void stopCustomWatching(FKObject* watched);
    void stopCustomWatching(FKObject* watched,const qint32 prop);
    void stopSharedWatching(FKObject* watched);
    void stopTotalWatching(FKObject* watched=0);
    void forbidTotalWatching();

    bool allowControlBy(FKObject* actor, const qint32 action);
    bool allowControlBy(FKObject* actor);
    bool allowEmitBy(FKObject* user);
    bool stopControl();
    bool forbidControl();
    bool forbidControlBy(FKObject* actor,const qint32 action);
    bool forbidControlBy(FKObject* actor);

    QVariant getProperty(const qint32 id)const;
    QVariant getValue(const qint32 id)const;
    void setProperty(const qint32 id,const QVariant& value);
    void setValue(const qint32 id,const QVariant& value);
public slots:
    bool forbidEmitBy(FKObject* user);
signals:
    void deletedFKObject(FKObject* obj);
protected:
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
    void doEvent(FKObject* target,const qint32 subject,const QVariant& value=QVariant(),FKObject* reciever=0);
    void doEvent(FKObject* target, const qint32 subject, FKObject* reciever);
    void doEvent(const qint32 target,const qint32 subject,const QVariant& value=QVariant(),FKObject* reciever=0);
    void doEvent(const qint32 target, const qint32 subject, FKObject* reciever);
    void doAction(const qint32 target,const qint32 subject,const QVariant& value=QVariant());
    void assignParentObjectId(const qint32 id);

    void logConstructor();
    void logDestructor();

//This section used by property and watching systems
    inline const QList<qint32>& eventList()const{return _classInfo->eventList();}
    inline const QList<qint32>& actionList()const{return _classInfo->actionList();}
    inline const QList<qint32>& commonPropertyList()const{return _classInfo->commonPropertyList();}
    inline const QList<qint32>& customPropertyList()const{return _classInfo->customPropertyList();}
public:
    inline const qint32& eventCount()const{return _classInfo->eventCount();}
    inline const qint32& actionCount()const{return _classInfo->actionCount();}
    inline const qint32& commonPropertyCount()const{return _classInfo->commonPropertyCount();}
    inline const qint32& customPropertyCount()const{return _classInfo->customPropertyCount();}

private:
    void updateCommon(const qint32 propertyId);
    void updateCustom(const qint32 propertyId);

//This section used by object manager
    void setId(const qint32 id){_id=id;}
    void setObjectManager(FKObjectManager* om){_om=om;}
    qint32 asquireParentObjectId()const;
    void processFKAction(FKEventObject* action);
    void processFKEvent(FKEventObject* event);
    friend class FKObjectManager;
    friend class FKObjectPool;

//This section used by watching system
    QSet<FKObject*> customVisors(const qint32 prop)const; //return visors for property. Used when updating custom property
    QSet<FKObject*> sharedVisors()const; //return visors of object's view; for user return itself. Used for vision changing. Returns cached set
    QSet<FKObject*> totalVisors()const; //return visors of object's view and of object itself. Used when updating common property. Calculates dynamically
    QSet<FKObject*> recalculatedSharedVisors()const;

    void addCommonVisors(QSet<FKObject*> newRecievers); //add newRecievers to shared watchers. Used when changing common vision
    void addSharedWatcher(FKObject* watcher);
    void addSharedVisors(QSet<FKObject*> newRecievers);
    void refreshProperty(const qint32 propertyId, const QSet<FKObject*>& recievers); //send property to recievers. Used when changing custom vision or values
    void initVisibleProperties(FKObject* watcher, const QList<qint32>& recievers); //send all properties visible for watcher to recievers. Used when changing shared vision
    void initCommonProperties(const QList<qint32>& recievers);

    void resetCustomWatching(FKObject* watcher);
    void removeVisors(QSet<FKObject*> oldRecievers);
    void removeTreeVisors(QSet<FKObject*> oldRecievers);

    bool watchingTest(const char* t, FKObject* watched)const;

//database access function
    FKDBIndex selfIndex()const;

    qint32 _id;
    FKObjectManager* _om;
    static FKFactory<FKObject> _factory;
    static FKObjectMetadata objectMetadata;
    static QReadWriteLock objectMetadataLocker;
    FKObjectInfoBase* _classInfo;
    mutable QMap<qint32,FKBaseProperty*> _properties;

//property system functions
    void executeEvent(const qint32 id, FKEventObject* ev=nullptr);
    void executeEvent(FKEventObject* ev);
    void executeAction(FKEventObject* ev);
    FKBaseProperty* findProperty(const qint32 id)const;
    FKEventExecutorBase* findEvent(const qint32 id,const QString& className)const;
    void installObjectInfo();

    virtual FKSystemObject* clone()const override;
    virtual bool packObject(QDataStream& stream)const override;
    virtual bool loadObject(QDataStream& stream)override;
};

#endif // FKOBJECT_H
