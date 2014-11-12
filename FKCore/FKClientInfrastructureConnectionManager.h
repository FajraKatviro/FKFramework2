#ifndef FKCLIENTINFRASTRUCTURECONNECTIONMANAGER_H
#define FKCLIENTINFRASTRUCTURECONNECTIONMANAGER_H

#include "FKConnectionManager.h"

class FKClientInfrastructure;

class FKClientInfrastructureConnectionManager : public FKConnectionManager{
    Q_OBJECT
public:
    FKClientInfrastructureConnectionManager(FKClientInfrastructure* infr, FKConnector* connector, QObject *parent=0);
    ~FKClientInfrastructureConnectionManager();
private:
    void processMessage(FKMessage* msg);
//    void processGuestEvent(FKBasicEvent* ev); - assume no guest connections from realm
    void processBasicEvent(FKBasicEvent* ev);
//    void processEvent(FKEvent* ev);           - assume no events from realm
//    void incomeMessageError(const QString& msgType,const QString& reason); - use default handler
    FKClientInfrastructure* _infr;
};

#endif
