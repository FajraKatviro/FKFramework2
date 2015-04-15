#ifndef FKLOCALCORE_H
#define FKLOCALCORE_H

#include "FKAbstractCore.h"

class FKCORESHARED_EXPORT FKLocalCore : public FKAbstractCore{
    Q_OBJECT
public:
    FKLocalCore();
    ~FKLocalCore();

    bool startRealm(const int /*port=0*/);
    bool startServer(const int /*port=0*/, const int /*realmPort=0*/,const QString& /*realmIP=QString()*/);
    bool startClientInfrastructure(const int /*realmPort=0*/,const QString& /*realmIP=QString()*/);
    //bool startUser(const QString& name){}
    bool createRoom(const QMap<QString,QVariant>& roomData){return true;/*todo*/}
    bool joinRoom(const qint32 id){return true;/*todo*/}

    bool stopRealm();
    bool stopServer(){return true;/*todo*/}
    bool stopClientManager(){return true;/*todo*/}
    bool stopClient(const qint32 number){return true;/*todo*/}
    bool quitRoom(){return true;/*todo*/}
};

#endif // FKLOCALCORE_H
