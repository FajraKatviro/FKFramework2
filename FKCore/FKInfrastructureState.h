#ifndef FKINFRASTRUCTURESTATE_H
#define FKINFRASTRUCTURESTATE_H

#include <QObject>

class FKInfrastructureStateValue;

enum class FKInfrastructureStateValueIdentifier:qint32{
    Off,
    On,
    Enabling,
    Disabling,
    ValuesCount
};

class FKInfrastructureState:public QObject{
    Q_OBJECT
public:
    FKInfrastructureState(const QString stateName=QString(),QObject* parent=0);
    ~FKInfrastructureState();
    bool accept(const qint32 val)const;
    bool decline(const qint32 val)const;
    void setState(const FKInfrastructureStateValueIdentifier state);
    void setState(const QString& childName,const FKInfrastructureStateValueIdentifier state);
signals:
    void stateChanged();
protected:
    FKInfrastructureStateValue* state(FKInfrastructureStateValueIdentifier s);
private:
    FKInfrastructureStateValueIdentifier _current;
    FKInfrastructureStateValue* _states[(qint32)FKInfrastructureStateValueIdentifier::ValuesCount];
};

#endif // FKINFRASTRUCTURESTATE_H
