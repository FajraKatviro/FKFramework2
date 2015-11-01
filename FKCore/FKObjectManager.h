#ifndef FKOBJECTMANAGER_H
#define FKOBJECTMANAGER_H

#include <QObject>

#include "fkcore_global.h"

#include "FKIDGenerator.h"
#include "FKObjectPool.h"

#include <QHash>

class FKObject;
class FKEventObject;
class FKDataBase;
class FKRoomModule;

class /*FKCORESHARED_EXPORT*/ FKObjectManager:public QObject{
    Q_OBJECT
public:
    FKObjectManager(QObject* parent=0);
    ~FKObjectManager();
    FKObject* genObject(const QString& className);
    FKObject* getObject(const qint32 id);
    void deleteObject(FKObject* obj);
    virtual qint32 roomContextId()const{return -1;}
    void setDataBase(FKDataBase* db){_db=db;}
    FKDataBase* dataBase()const{return _db;}
public slots:
    void internalEvent(FKEventObject* event);
    void internalAction(FKEventObject* action);
    void shareObject(FKObject* obj,const QList<qint32>& recievers);
    void unshareObject(FKObject* obj,const QList<qint32>& recievers);

    void externalEvent(FKEventObject* event);
    void externalAction(FKEventObject* action);
    void createObject(const QVariant& data);
    void deleteObject(const QVariant& data);
private:
    FKObject* genObject(const QString& className,const qint32 id,const bool createServant=false);
    FKIDGenerator _idGenerator;
    FKObjectPool _objectPool;
    QHash<qint32,FKObject*> _objects;
    FKDataBase* _db;
};

#endif // FKOBJECTMANAGER_H
