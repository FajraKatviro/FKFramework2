#ifndef FKUSERINFRASTRUCTURE_H
#define FKUSERINFRASTRUCTURE_H

#include "FKInfrastructure.h"
#include <QSharedPointer>

class FKConnectionManager;
class FKObjectManager;
class FKEventObject;
class FKWorld;
//class FKInteractiveModel;

class FKUserInfrastructure : public FKInfrastructure{
    Q_OBJECT
public:
    FKUserInfrastructure(const qint32 id,FKWorld* worldObject,QObject* parent=0);
    ~FKUserInfrastructure();
    FKInfrastructureType infrastructureType()const;
private slots:
    void createObjectRequest(qint32 reciever,QVariant request);
    void deleteObjectRequest(qint32 reciever,QVariant request);
    void incomeEvent(qint32 reciever,FKEventObject* event);
private:
    FKObjectManager* _om=nullptr;
    FKWorld* _world;
    //FKInteractiveModel* _im=nullptr;
    const qint32 _id;
};

#endif // FKUSERINFRASTRUCTURE_H
