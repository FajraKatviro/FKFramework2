#ifndef _FK_APPLICATION_
#define _FK_APPLICATION_

#include <QObject>
#include <QtQml>

#include "fkcore_global.h"

class FKCORESHARED_EXPORT FKApplication:public QObject{
    Q_OBJECT
public:
    FKApplication();
    ~FKApplication();
    bool start();
    template <class FKCore> bool setCore();
    template <class FKUI> bool setUserInterface();
    bool setInterfaceBridge(const QString& bridgeType);
private:
    virtual void prepare();
    virtual void initLayer();
    bool _interface;
    bool _core;
    QString _interfaceBridge;
    QQmlApplicationEngine* _qmlEngine;
};

template<class FKCore>
bool FKApplication::setCore(){
    if(!_core){
        _core=true;
        qmlRegisterType<FKCore>("FKApp",1,0,"Core");
        return true;
    }
    return false;
}

template<class FKUI>
bool FKApplication::setUserInterface(){
    if(!_interface){
        _interface=true;
        qmlRegisterType<FKUI>("FKApp",1,0,"UserInterface");
        return true;
    }
    return false;
}

#endif
