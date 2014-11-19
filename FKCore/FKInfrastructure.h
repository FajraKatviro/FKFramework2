#ifndef FKINFRASTRUCTURE_H
#define FKINFRASTRUCTURE_H

#include <QObject>
#include <QMap>

enum class FKInfrastructureType:qint8{
    Realm,
    Server,
    Client,
    User,
    UserSlot
};

class FKInfrastructure : public QObject{
    Q_OBJECT
public:
    FKInfrastructure(QObject* parent=0);
    ~FKInfrastructure();
    virtual FKInfrastructureType infrastructureType()const=0;
    bool waitingForAnswer(FKInfrastructureType t)const;
signals:
    void waitingForAnswerChanged(FKInfrastructureType infr);
    void messageRequested(const QString msg);
protected:
    bool requestAnswer(FKInfrastructureType t,const QString& subject);
    bool submitAnswer(FKInfrastructureType t,const QString& subject);
    void cancelAnswer(FKInfrastructureType t);
private:
    QMap<FKInfrastructureType,QString> _requests;
};

#endif // FKINFRASTRUCTURE_H
