#ifndef FKINFRASTRUCTURESTATEVALUE_H
#define FKINFRASTRUCTURESTATEVALUE_H

#include <QObject>

class FKInfrastructureState;

class FKInfrastructureStateValue:public QObject{
    Q_OBJECT
public:
    FKInfrastructureStateValue(QObject* parent=0);
    ~FKInfrastructureStateValue();
    void initState();
    bool accept(const qint32 value)const;
    bool decline(const qint32 value)const;

    void addSubstate(FKInfrastructureState* subState);
    void addDecline(const qint32 val);
    void addAccept(const qint32 val);
signals:
    void initRequested(FKInfrastructureStateValue* val);
private:
    QList<qint32> _decline;
    QList<qint32> _accept;
    QList<FKInfrastructureState*> _substates;
    bool _initialized;
};

#endif // FKINFRASTRUCTURESTATEVALUE_H
