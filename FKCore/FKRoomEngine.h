#ifndef FKROOMENGINE_H
#define FKROOMENGINE_H

#include <QObject>
#include <QQmlListProperty>

#include "fkcore_global.h"

class FKRoomContext;
class QQmlComponent;
class FKRoomModule;
class FKEventObject;
class FKInstructionObject;
class FKMessage;
class FKBasicEvent;
class FKVersionList;

class FKCORESHARED_EXPORT FKRoomEngine : public QObject{
    Q_OBJECT
    Q_PROPERTY(QObject* contextManager READ contextManager NOTIFY contextManagerChanged)
    Q_PROPERTY(FKRoomContext* serverContext READ serverContext NOTIFY serverContextChanged)
    Q_PROPERTY(FKRoomContext* userContext READ userContext NOTIFY userContextChanged)
    Q_PROPERTY(QQmlListProperty<QObject> roomContextItems READ roomContextItems NOTIFY roomContextItemsChanged)
public:
    explicit FKRoomEngine(QObject *parent = 0);
    ~FKRoomEngine();
    QObject* contextManager()const;
    FKRoomContext* serverContext()const;
    FKRoomContext* userContext()const;
    QQmlListProperty<QObject> roomContextItems()const;
public slots:
    void loadModule(const QString moduleName);
    void releaseModule();

    void createContext(const qint32 rootId, qint8 flags);
    void releaseContext(const qint32 rootId);

//    void processAction(FKEventObject* ev);
//    void processEvent(FKEventObject* ev);
//    void processInstruction(FKInstructionObject* instruction);
signals:
    void contextManagerChanged();
    void serverContextChanged();
    void userContextChanged();
    void roomContextItemsChanged();

    void moduleLoaded(const FKVersionList version);
    void contextCreated(const qint32 rootId);

    void actionDispatched(FKEventObject* ev);
    void eventDispatched(FKEventObject* ev);
    void messageDispatched(FKMessage* msg);
    void instructionDispatched(FKInstructionObject* instruction);
    void notificationDispatched(FKBasicEvent* ev);
private:
    static FKRoomContext* getContextItem(QQmlListProperty<QObject>* prop,int index);
    static int countContexts(QQmlListProperty<QObject>* prop);
    QMap<qint32,FKRoomContext*> _contexts;
    qint32 _userContextId=-1;
    qint32 _serverContextId=-1;
    QObject* _contextManager=nullptr;
    QQmlComponent* _contextComponent;
    FKRoomModule* _roomModule;
};

#endif // FKROOMENGINE_H
