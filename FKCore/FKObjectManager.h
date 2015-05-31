#ifndef FKOBJECTMANAGER_H
#define FKOBJECTMANAGER_H

#include <QObject>

#include "fkcore_global.h"

#include "FKIDGenerator.h"

#include <QHash>

class FKObject;
class FKEvent;
class FKMessage;
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
    virtual void internalEvent(FKEvent* event);
    virtual void externalEvent(FKEvent* event);
    virtual void internalAction(FKEvent* action);
    virtual void externalAction(FKEvent* action);
    virtual void internalMessageRequest(const QString& msg, const QList<qint32>& recievers);
    virtual void externalMessageRequest(FKMessage* message, const qint32 sender);
    virtual void shareObject(FKObject* obj,const QList<qint32>& recievers);
    virtual void createObject(const QVariant& data);
    virtual void unshareObject(FKObject* obj,const QList<qint32>& recievers);
    virtual void deleteObject(const QVariant& data);
    virtual qint32 clientId()const{return -1;}
    void setDataBase(FKDataBase* db){_db=db;}
    FKDataBase* dataBase()const{return _db;}
    void setRoomModule(FKRoomModule* m){_roomModule=m;}
    FKRoomModule* roomModule()const{return _roomModule;}
private:
    FKObject* genObject(const QString& className,const qint32 id);
    FKIDGenerator _idGenerator;
    QHash<qint32,FKObject*> _objects;
    FKDataBase* _db;
    FKRoomModule* _roomModule;
};

#endif // FKOBJECTMANAGER_H
