#ifndef FKUSERINTERFACE_H
#define FKUSERINTERFACE_H

#include "fkcore_global.h"

#include <QObject>

class FKCORESHARED_EXPORT FKUserInterface:public QObject{
    Q_OBJECT
public:
    FKUserInterface(QObject* parent);
    ~FKUserInterface();
public slots:
    virtual void showMessage(const QString& m)=0;
    virtual void show()=0;
signals:
    void quitApplicationRequested();
    void showMessageRequested(QString msg);
    void startRealmRequested();
    void startClientInfrastructureRequested();
    void createClientRequested(const QString clientName,const QString password);
    void clientLoginRequested(const QString clientName,const QString password);
    void createUserRequested(const QString userName);
    void deleteUserRequested(const QString userName);
protected:
    void requestCreateUser(const QString& arg);
    void requestDeleteUser(QString arg);
};

#endif
