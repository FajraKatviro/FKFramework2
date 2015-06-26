#ifndef FKREALMCONNECTIONMANAGER_H
#define FKREALMCONNECTIONMANAGER_H

#include "FKConnectionManager.h"

class FKRealm;

class FKRealmConnectionManager : public FKConnectionManager{
    Q_OBJECT
public:
    FKRealmConnectionManager(FKRealm* realm,FKConnector* connector,QObject* parent=0);
    ~FKRealmConnectionManager();
protected:
    FKRealm* realm()const{return _realm;}
private:
    void processMessage(FKMessage* msg);
    void processGuestEvent(FKBasicEvent* ev);
    void processBasicEvent(FKBasicEvent* ev);
    void processEvent(FKEventObject* ev);
    void incomeMessageError(const QString& msgType,const QString& reason);
    FKRealm* _realm;
};

class FKRealmConnectionManagerC:public FKRealmConnectionManager{
    Q_OBJECT
public:
    FKRealmConnectionManagerC(const QString& id,FKRealm* realm,FKConnector* connector,QObject* parent=0);
    ~FKRealmConnectionManagerC();
private:
    void processMessage(FKMessage* msg);
    void processGuestEvent(FKBasicEvent* ev);
    void processBasicEvent(FKBasicEvent* ev);
    void processEvent(FKEventObject* ev);
    void incomeMessageError(const QString& msgType,const QString& reason);
    QString _id;
};

class FKRealmConnectionManagerS:public FKRealmConnectionManager{
    Q_OBJECT
public:
    FKRealmConnectionManagerS(const qint32 id,FKRealm* realm,FKConnector* connector,QObject* parent=0);
    ~FKRealmConnectionManagerS();
private:
    void processMessage(FKMessage* msg);
    void processGuestEvent(FKBasicEvent* ev);
    void processBasicEvent(FKBasicEvent* ev);
    void processEvent(FKEventObject* ev);
    void incomeMessageError(const QString& msgType,const QString& reason);
    qint32 _id;
};

#endif // FKREALMCONNECTIONMANAGER_H
