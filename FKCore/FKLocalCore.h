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
    bool createRoom(const QMap<QString,QVariant>& roomData){}
    bool joinRoom(const qint32 id){}

    bool stopRealm();
    bool stopServer(){}
    bool stopClientManager(){}
    bool stopClient(const qint32 number){}
    bool quitRoom(){}
};

#endif // FKLOCALCORE_H
