#ifndef FKUSERINFRASTRUCTURESLOT_H
#define FKUSERINFRASTRUCTURESLOT_H

#include <QObject>

class FKUserInfrastructureAlias;
class FKConnector;
class FKEventObject;
class FKServerConnectionManagerU;

class FKUserInfrastructureSlot:public QObject{
    Q_OBJECT
public:
    FKUserInfrastructureSlot(FKUserInfrastructureAlias* alias,const qint32 userObjectId,const QString& invitePassword);
    ~FKUserInfrastructureSlot();
    QString password()const;
    bool isActive()const;
    void setActive(const bool t);

    void dropUser();

    void setUserConnector(FKConnector* connector);

    void sendMessage(const QString msg);
    void sendEvent(FKEventObject* event);
    void sendData(const QByteArray& data);
    void incomeAction(FKEventObject* action);
signals:
    void gotAction(FKEventObject* action);
private:
    const qint32 _id;
    const QString _password;
    FKServerConnectionManagerU* _connection;
    FKUserInfrastructureAlias* _client;
    bool _active;
};

class FKUserInfrastructureAlias{
public:
    FKUserInfrastructureAlias(const QString& clientId);
    FKUserInfrastructureAlias(const FKUserInfrastructureAlias& other);
    ~FKUserInfrastructureAlias();
    bool isActive()const;
    void addUser(FKUserInfrastructureSlot* user);
    void dropAlias();
private:
    const QString _id;
    QList<FKUserInfrastructureSlot*> _users;
};

#endif // FKUSERINFRASTRUCTURESLOT_H
