#ifndef FKROOMCONTEXT_H
#define FKROOMCONTEXT_H

#include <QObject>

class FKObjectManager;
class FKRoom;
class FKUser;
class FKObject;
class FKRoomInviteData;
class FKEventObject;
class FKBasicEvent;

class FKRoomContext : public QObject{
    Q_OBJECT
    Q_PROPERTY(FKObject* roomObject READ roomObject NOTIFY roomObjectChanged)
    Q_PROPERTY(FKObject* userObject READ userObject NOTIFY userObjectChanged)
    Q_PROPERTY(QObject*  rootEntity READ rootEntity NOTIFY rootEntityChanged)
public:
    explicit FKRoomContext(QObject *parent = 0);
    ~FKRoomContext();
    FKObject* roomObject()const;
    FKObject* userObject()const;
    QObject*  rootEntity()const;

    void setRoomObject(const qint32 objectId);
    void setUserObject(const qint32 objectId);
    void setRootEntity(QObject* entity);

    virtual bool addClient(const FKRoomInviteData& invite);
    virtual bool removeClient(const FKRoomInviteData& invite);

    void createObjects(const QVariant& data);
    void deleteObjects(const QVariant& data);
    void processAction(FKEventObject* ev);
    void processEvent(FKEventObject* ev);
signals:
    void roomObjectChanged();
    void userObjectChanged();
    void rootEntityChanged();

    void eventDispatched(FKEventObject* ev);
    void actionDispatched(FKEventObject* ev);
    void notificationDispatched(FKBasicEvent* ev);
private:
    QObject* _rootEntity=nullptr;
    FKRoom* _room=nullptr;
    FKUser* _user=nullptr;
    FKObjectManager* _om=nullptr;
};

#endif // FKROOMCONTEXT_H
