#ifndef FKROOMENGINE_H
#define FKROOMENGINE_H

#include <QObject>
#include <QQmlListProperty>

#include "FKVersionList.h"
#include "FKInstructionObject.h"

#include "fkcore_global.h"

class FKRoomContext;
class QQmlComponent;
class FKRoomModule;
class FKEventObject;
class FKBasicEvent;

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
    QQmlListProperty<QObject> roomContextItems();
public slots:
    void processAction(FKEventObject* ev);
    void processEvent(FKEventObject* ev);
    void processInstruction(FKInstructionObject instruction);
signals:
    void contextManagerChanged();
    void serverContextChanged();
    void userContextChanged();
    void roomContextItemsChanged();

    void messageRequested(QString msg);
    void instructionDispatched(FKInstructionObject notification);
private slots:
    void cancelEvent(QObject* ev);
private:
    FKVersionList loadModule(const QString moduleName);
    void releaseModule();
    FKRoomContext* createContext(const qint32 rootId, qint8 flags);
    void releaseContext(const qint32 rootId);

    static QObject *getContextItem(QQmlListProperty<QObject>* prop,int index);
    static int countContexts(QQmlListProperty<QObject>* prop);
    QMap<qint32,FKRoomContext*> _contexts;
    qint32 _userContextId=-1;
    qint32 _serverContextId=-1;
    QObject* _contextManager=nullptr;
    QQmlComponent* _contextComponent;
    FKRoomModule* _roomModule;
};

#endif // FKROOMENGINE_H
