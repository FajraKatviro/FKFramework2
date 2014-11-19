#ifndef FKUSERINFRASTRUCTURESLOT_H
#define FKUSERINFRASTRUCTURESLOT_H

#include "FKInfrastructure.h"

class FKConnector;
class FKServerConnectionManagerU;
class FKEvent;
class FKClientInfrastructureSlot;

//class FKUserInfrastructureSlot:public FKInfrastructure{
//    Q_OBJECT
//public:
//    FKUserInfrastructureSlot(FKClientInfrastructureSlot* client,const qint32 id,const QString& password,QObject* parent=0);
//    ~FKUserInfrastructureSlot();
//    virtual FKInfrastructureType infrastructureType()const;
//    QString password()const;
//    bool isActive()const;

//    void dropUser();

//    void setUserConnector(FKConnector* connector);

//    //void sendMessage(const QString msg);
//    //void sendEvent(FKEvent* event);
//    //void sendData(const QByteArray data);
//signals:

//public slots:
//private:
//    const qint32 _id;
//    const QString _password;
//    FKServerConnectionManagerU* _connection;
//    FKClientInfrastructureSlot* _client;
//};

//class FKClientInfrastructureSlot:public QObject{
//    Q_OBJECT
//public:
//    FKClientInfrastructureSlot(QObject* parent=0);
//    ~FKClientInfrastructureSlot();
//    void addUser(FKUserInfrastructureSlot* user);
//    void dropClient();
//private:
//    QList<FKUserInfrastructureSlot*> _users;
//};

#endif // FKUSERINFRASTRUCTURESLOT_H
