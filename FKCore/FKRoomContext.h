#ifndef FKROOMCONTEXT_H
#define FKROOMCONTEXT_H

#include <QObject>
#include <QHash>

#include "FKIDGenerator.h"

class FKRoom;
class FKUser;
class FKObject;
class FKRoomInviteData;
class FKEventObject;
class FKBasicEvent;
class FKDataBase;

class FKRoomContext : public QObject{
    Q_OBJECT
    Q_PROPERTY(FKObject* roomObject READ roomObject NOTIFY roomObjectChanged)
    Q_PROPERTY(FKObject* userObject READ userObject NOTIFY userObjectChanged)
    Q_PROPERTY(QObject*  rootEntity READ rootEntity NOTIFY rootEntityChanged)
public:
    explicit FKRoomContext(QObject *parent = 0);
    ~FKRoomContext();

    //QML interaction
    FKObject* roomObject()const;
    FKObject* userObject()const;
    QObject*  rootEntity()const;

    //FKRoomEngine interaction
    void setRoomObject(const qint32 objectId);
    void setUserObject(const qint32 objectId);
    void setRootEntity(QObject* entity);

    virtual bool addClient(const FKRoomInviteData& invite);
    virtual bool removeClient(const FKRoomInviteData& invite);

    void createObject(const QVariant& data);
    void deleteObject(const QVariant& data);
    void processAction(FKEventObject* ev);
    void processEvent(FKEventObject* ev);

    //FKObject interaction
    FKObject* genObject(const QString& className);
    FKObject* getObject(const qint32 id);
    void deleteObject(FKObject* obj);
    virtual qint32 roomContextId()const{return -1;}
    void setDataBase(FKDataBase* db){_db=db;}
    FKDataBase* dataBase()const{return _db;}

    void internalEvent(FKEventObject* event);
    void internalAction(FKEventObject* action);
    void shareObject(FKObject* obj,const QList<qint32>& recievers);
    void unshareObject(FKObject* obj,const QList<qint32>& recievers);
signals:
    //QML notification
    void roomObjectChanged();
    void userObjectChanged();
    void rootEntityChanged();

    //Infrastructure notification
    void eventDispatched(FKEventObject* ev);
    void actionDispatched(FKEventObject* ev);
    void notificationDispatched(FKBasicEvent* ev);

private:
    FKObject* genObject(const QString& className,const qint32 id,const bool createServant=false);
    FKIDGenerator _idGenerator;
    QHash<qint32,FKObject*> _objects;
    FKDataBase* _db=nullptr;
    QObject* _rootEntity=nullptr;
    FKRoom* _room=nullptr;
    FKUser* _user=nullptr;
};

#endif // FKROOMCONTEXT_H
