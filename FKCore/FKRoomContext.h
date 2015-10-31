#ifndef FKROOMCONTEXT_H
#define FKROOMCONTEXT_H

#include <QObject>

class FKObjectManager;
class FKRoom;
class FKUser;
class FKObject;
class FKRoomInviteData;
class FKEventObject;

class FKRoomContext : public QObject{
    Q_OBJECT
    Q_PROPERTY(FKObject* roomObject READ roomObject NOTIFY roomObjectChnaged)
    Q_PROPERTY(FKObject* userObject READ userObject NOTIFY userObjectChnaged)
    Q_PROPERTY(QObject*  rootEntity READ rootEntity NOTIFY rootEntityChanged)
    Q_PROPERTY(bool syncComplete READ isSyncComplete NOTIFY syncCompleteChanged)
public:
    explicit FKRoomContext(QObject *parent = 0);
    ~FKRoomContext();
    FKObject* roomObject()const;
    FKObject* userObject()const;
    QObject*  rootEntity()const;
    bool  isSyncComplete()const;

    bool createRoomObject(const QString& roomClass);
    void setRootEntity(QObject* entity);

    virtual bool addClient(const FKRoomInviteData& invite);
    virtual void completeSync();

    void postAction(FKEventObject* ev);
    void postEvent(FKEventObject* ev);
signals:
    void roomObjectChnaged();
    void userObjectChnaged();
    void rootEntityChanged();
    void syncCompleteChanged();

    void eventDispatched(FKEventObject* ev);
    void actionDispatched(FKEventObject* ev);
    void messageDispatched(FKEventObject* ev);
protected slots:
    virtual void onObjectCreated(FKObject* newObject);
protected:
    virtual void installObjectManager(FKObjectManager* om);
private slots:
    void eventRecycler(FKEventObject* ev);
private:
    void installObjectManager();
    void setUser(const qint32 id);
    QObject* _rootEntity=nullptr;
    FKRoom* _room=nullptr;
    FKUser* _user=nullptr;
    FKObjectManager* _om=nullptr;
    bool _syncComplete=false;
    qint32 _userId=-1;
};

#endif // FKROOMCONTEXT_H
