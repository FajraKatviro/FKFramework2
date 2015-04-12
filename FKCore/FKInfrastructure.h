#ifndef FKINFRASTRUCTURE_H
#define FKINFRASTRUCTURE_H

#include <QObject>
#include <QMap>

class FKDataBase;

enum class FKInfrastructureType:qint8{
    Realm,
    Server,
    Client,
    User
};

class FKInfrastructure : public QObject{
    Q_OBJECT
public:
    FKInfrastructure(QObject* parent=0);
    ~FKInfrastructure();
    virtual FKInfrastructureType infrastructureType()const=0;
    bool waitingForAnswer(FKInfrastructureType t)const;
    void setDataBase(FKDataBase* db);
    static QString createRandomString(const qint32 minLen, const qint32 maxLen);
signals:
    void waitingForAnswerChanged(FKInfrastructureType infr);
    void messageRequested(const QString msg);
protected:
    bool requestAnswer(FKInfrastructureType t,const QString& subject);
    bool submitAnswer(FKInfrastructureType t,const QString& subject);
    void cancelAnswer(FKInfrastructureType t);
    FKDataBase* database();
private:
    QMap<FKInfrastructureType,QString> _requests;
    FKDataBase* _db;
};

#endif // FKINFRASTRUCTURE_H
