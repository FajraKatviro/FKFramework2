#ifndef FKLOCALCONNECTOR_H
#define FKLOCALCONNECTOR_H

#include "FKConnector.h"

class FKLocalConnector : public FKConnector{
    Q_OBJECT
public:
    FKLocalConnector(QObject* parent=0);
    ~FKLocalConnector();
    void sendMessage(const QString& msgType, FKSystemObject* msg);
    void sendMessage(QByteArray data);
    void join(FKLocalConnector* other);
signals:
    void transmitMessage(const QByteArray& data);
    void transmitMessage(const QString& msgType, FKSystemObject* msg);
private slots:
    void readMessage(const QByteArray& data);
    void readMessage(const QString& msgType, FKSystemObject* msg);
};

#endif // FKLOCALCONNECTOR_H
