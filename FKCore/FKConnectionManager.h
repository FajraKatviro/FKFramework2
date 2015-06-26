#ifndef FKCONNECTIONMANAGER_H
#define FKCONNECTIONMANAGER_H

#include <QObject>

class FKSystemObject;
class FKMessage;
class FKBasicEvent;
class FKEventObject;
class FKConnector;

class FKConnectionManager : public QObject{
    Q_OBJECT
public:
    FKConnectionManager(FKConnector* connector,QObject* parent=0);
    ~FKConnectionManager();
    FKConnector* connector()const{return _connector;}
    bool isActive()const;
    void sendData(const QByteArray& data);
    void sendMessage(FKMessage* message);
    void sendMessage(const QString& message);
    void sendEvent(FKEventObject* event);
    void sendGuestEvent(FKBasicEvent* event);
    void sendBasicEvent(FKBasicEvent* event);
    void dropConnection();
    QString address()const;
signals:
    void connectionStatusChanged();
private slots:
    void processMsg(const QString msgType, FKSystemObject* msg);
private slots:
    void connectionStatusChangedSlot();
private:
    virtual void processMessage(FKMessage* msg);
    virtual void processGuestEvent(FKBasicEvent* ev);
    virtual void processBasicEvent(FKBasicEvent* ev);
    virtual void processEvent(FKEventObject* ev);
    virtual void incomeMessageError(const QString& msgType,const QString& reason);
    FKConnector* _connector;
};

#endif // FKCONNECTIONMANAGER_H
