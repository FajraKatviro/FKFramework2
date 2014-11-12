#ifndef FKOBJECTMANAGER_H
#define FKOBJECTMANAGER_H

#include <QObject>

#include "fkcore_global.h"

#include "FKIDGenerator.h"

#include <QHash>
#include <QMutex>

class FKObject;
class FKEvent;
class FKMessage;
class FKDataBase;

class /*FKCORESHARED_EXPORT*/ FKObjectManager:public QObject{
    Q_OBJECT
public:
    FKObjectManager(QObject* parent=0);
    ~FKObjectManager()=0;
    FKObject* getObject(const qint32 id);
    void regObject(FKObject* obj);
    void freeObject(const qint32 id);
    virtual void internalEvent(FKEvent* event);
    virtual void externalEvent(FKEvent* event);
    virtual void internalAction(FKEvent* action);
    virtual void externalAction(FKEvent* action);
    virtual void internalMessageRequest(const QString& msg, const QList<qint32>& recievers);
    virtual void externalMessageRequest(FKMessage* message, const qint32 sender);
    virtual void shareObject(FKObject* obj,const QList<qint32>& recievers);
    virtual void unshareObject(FKObject* obj,const QList<qint32>& recievers);
    virtual qint32 clientId()const{return -1;}
    void setDataBase(FKDataBase* db){_db=db;}
    FKDataBase* dataBase()const{return _db;}
protected:
    void genObject(const QString& className,const qint32 id);
    void setRootObject(FKObject* obj){_rootObject=obj;}
    FKObject* rootObject()const{return _rootObject;}
private:
    void addObject(FKObject* obj);
    FKIDGenerator _idGenerator;
    QHash<qint32,FKObject*> _objects;
    QMutex _mutex;
    FKObject* _rootObject;
    FKDataBase* _db;
};

#endif // FKOBJECTMANAGER_H
