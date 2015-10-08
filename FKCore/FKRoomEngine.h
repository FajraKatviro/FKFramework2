#ifndef FKROOMENGINE_H
#define FKROOMENGINE_H

#include <QObject>
#include <QQmlListProperty>

class FKRoomContext;
class QQmlComponent;
class FKRoomModule;
class FKEventObject;
class FKInstructionObject;
class FKMessage;
class FKBasicEvent;

class FKRoomEngine : public QObject{
    Q_OBJECT
    Q_PROPERTY(FKRoomContext* serverContext READ serverContext NOTIFY serverContextChanged)
    Q_PROPERTY(FKRoomContext* userContext READ defaultContext NOTIFY defaultContextChanged)
    Q_PROPERTY(QQmlListProperty<FKRoomContext> userContexts READ userContexts NOTIFY userContextsChanged)
public:
    explicit FKRoomEngine(QObject *parent = 0);
    ~FKRoomEngine();
    FKRoomContext* serverContext()const;
    //FKRoomContext* userContext(const qint32 rootId)const;
    FKRoomContext* defaultContext()const;
    QQmlListProperty<FKRoomContext> userContexts()const;
public slots:
//    void loadModule(const QString& moduleName);
//    void releaseModule();
//    void createServerContext();
//    void releaseServerContext();
//    void createUserContext(const qint32 rootId, bool defaultContext);

//    void processAction(FKEventObject* ev);
//    void processEvent(FKEventObject* ev);
//    void processMessage(FKMessage* msg);
//    void processInstruction(FKInstructionObject* instruction);
signals:
    void defaultContextChanged();
    void userContextsChanged();
    void serverContextChanged();

    void actionDispatched(FKEventObject* ev);
    void eventDispatched(FKEventObject* ev);
    void messageDispatched(FKMessage* msg);
    void instructionDispatched(FKInstructionObject* instruction);
    void notificationDispatched(FKBasicEvent* ev);
private:
    static FKRoomContext* userContext(QQmlListProperty<FKRoomContext>* prop,int index);
    static int countUserContexts(QQmlListProperty<FKRoomContext>* prop);
    FKRoomContext* _serverContext=nullptr;
    QMap<qint32,FKRoomContext*> _userContexts;
    qint32 _defaultContext=-1;
    QQmlComponent* _userContextComponent;
    QQmlComponent* _serverContextComponent;
    FKRoomModule* _roomModule;
};

#endif // FKROOMENGINE_H
