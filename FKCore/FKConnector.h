#ifndef FKCONNECTOR_H
#define FKCONNECTOR_H

#include <QObject>
class FKSystemObject;

class FKConnector : public QObject{
    Q_OBJECT
public:
    FKConnector(QObject* parent=0);
    ~FKConnector();
    virtual void sendMessage(const QString& msgType, FKSystemObject* msg)=0;
    virtual void sendMessage(QByteArray data)=0;
    enum Status{Connected,Disconnected};
    FKConnector::Status status()const{return _status;}
    virtual QString address()const{return QString();}
    void activate();
signals:
    void gotMessage(const QString msgType, FKSystemObject* msg);
    void statusChanged(FKConnector::Status status);
protected slots:
    void setStatus(FKConnector::Status status);
private:
    Status _status;
};
Q_DECLARE_METATYPE(FKConnector::Status)

#endif // FKCONNECTOR_H
