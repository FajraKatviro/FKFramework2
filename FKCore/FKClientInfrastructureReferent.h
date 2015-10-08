#ifndef FKCLIENTINFRASTRUCTUREREFERENT_H
#define FKCLIENTINFRASTRUCTUREREFERENT_H

#include <QObject>

class FKConnector;
class FKBasicEvent;
class FKEventObject;
class FKUpdateData;
class FKServerConnectionManagerU;

class FKClientInfrastructureReferent:public QObject{
    Q_OBJECT
public:
    FKClientInfrastructureReferent(const QString& clientId,const QString& password);
    ~FKClientInfrastructureReferent();
    void setPassword(const QString& password);
    bool isActive()const;
    bool isInvited()const;
    bool isConnected()const;
    void setInvited();
    void setConnected(FKConnector* connector);
    void addUser(const qint32 userId);
    void dropConnection();
    QList<qint32> users()const;
    QString password()const;
    void sendBasicEvent(FKBasicEvent* ev);
    void pushEvent(FKEventObject* ev);
    void sendEvent();
    void sendUpdateData(FKUpdateData* ev);
signals:
    void actionRequested(FKEventObject* ev);
    void updateRequested(FKUpdateData* ev);
    void inviteTimeout();
private:
    void createConnectionManager(FKConnector* connector);
    void incomeAction(FKEventObject* ev);
    const QString _id;
    QString _password;
    bool _invited=false;
    bool _connected=false;
    FKServerConnectionManagerU* _connection=nullptr;
    QList<qint32> _users;
    FKEventObject* _event=nullptr;
};

#endif // FKCLIENTINFRASTRUCTUREREFERENT_H
