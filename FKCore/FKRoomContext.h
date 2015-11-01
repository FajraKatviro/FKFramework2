#ifndef FKROOMCONTEXT_H
#define FKROOMCONTEXT_H

#include <QObject>

class FKObjectManager;
class FKRoom;
class FKUser;
class FKObject;
class FKRoomInviteData;
class FKEventObject;
class FKInstructionObject;
class FKBasicEvent;

class FKRoomContext : public QObject{
    Q_OBJECT
    Q_PROPERTY(FKObject* roomObject READ roomObject NOTIFY roomObjectChanged)
    Q_PROPERTY(FKObject* userObject READ userObject NOTIFY userObjectChanged)
    Q_PROPERTY(QObject*  rootEntity READ rootEntity NOTIFY rootEntityChanged)
    Q_PROPERTY(bool syncComplete READ isSyncComplete NOTIFY syncCompleteChanged)
public:
    explicit FKRoomContext(QObject *parent = 0);
    ~FKRoomContext();
    FKObject* roomObject()const;
    FKObject* userObject()const;
    QObject*  rootEntity()const;
    bool  isSyncComplete()const;

    void setRootEntity(QObject* entity);

    void processAction(FKEventObject* ev);
    void processEvent(FKEventObject* ev);
    void processInstruction(FKInstructionObject* instruction);
signals:
    void roomObjectChanged();
    void userObjectChanged();
    void rootEntityChanged();
    void syncCompleteChanged();

    void eventDispatched(FKEventObject* ev);
    void actionDispatched(FKEventObject* ev);
    void instructionDispatched(FKInstructionObject instruction);
    void notificationDispatched(FKBasicEvent* ev);
protected slots:
    virtual void onObjectCreated(FKObject* newObject);
protected:
    virtual void installObjectManager(FKObjectManager* om);
private:
    virtual void completeSync();
    virtual bool addClient(const FKRoomInviteData& invite);
    bool createRoomObject(const QString& roomClass);

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
