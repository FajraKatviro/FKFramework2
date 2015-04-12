#ifndef FKSERVERCONNECTIONMANAGER_H
#define FKSERVERCONNECTIONMANAGER_H

#include "FKConnectionManager.h"

class FKServerInfrastructure;
class FKUserInfrastructureSlot;

//class FKServerConnectionManager:public FKConnectionManager{
//    Q_OBJECT
//public:
//    FKServerConnectionManager(FKServerInfrastructure* server,FKConnector* connector,QObject* parent=0);
//    ~FKServerConnectionManager();
//private:
//    void processMessage(FKMessage* msg);
//    void processGuestEvent(FKBasicEvent* ev);
//    void processBasicEvent(FKBasicEvent* ev);
//    void processEvent(FKEvent* ev);
//    void incomeMessageError(const QString& msgType,const QString& reason);
//    FKServerInfrastructure* _server;
//};

class FKServerConnectionManagerR:public FKConnectionManager{
    Q_OBJECT
public:
    FKServerConnectionManagerR(FKServerInfrastructure* server,FKConnector* connector,QObject* parent=0);
    ~FKServerConnectionManagerR();
private:
    void processMessage(FKMessage* msg);
    void processGuestEvent(FKBasicEvent* ev);
    void processBasicEvent(FKBasicEvent* ev);
    void processEvent(FKEvent* ev);
    void incomeMessageError(const QString& msgType,const QString& reason);
    FKServerInfrastructure* _server;
};

class FKServerConnectionManagerU:public FKConnectionManager{
    Q_OBJECT
public:
    FKServerConnectionManagerU(FKUserInfrastructureSlot* userSlot,FKConnector* connector,QObject* parent=0);
    ~FKServerConnectionManagerU();
private:
    void processMessage(FKMessage* msg);
    void processGuestEvent(FKBasicEvent* ev);
    void processBasicEvent(FKBasicEvent* ev);
    void processEvent(FKEvent* ev);
    void incomeMessageError(const QString& msgType,const QString& reason);
    FKUserInfrastructureSlot* _user;
};

#endif
