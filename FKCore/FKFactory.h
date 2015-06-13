#ifndef FKFACTORY_H
#define FKFACTORY_H

#include <QObject>
#include <QMetaObject>
#include <QMap>
#include <QReadWriteLock>

#include "fkcore_global.h"


struct FKAbstractFactoryObjectCreator{
    virtual ~FKAbstractFactoryObjectCreator(){}
    virtual QObject* newInstance()const=0;
};

template<class D>
struct FKFactoryObjectCreator:public FKAbstractFactoryObjectCreator{
    virtual QObject* newInstance()const override{return new D;}
};

template<class D>
class /*FKCORESHARED_EXPORT*/ FKFactoryBase{
    QMap<QString,FKAbstractFactoryObjectCreator*> _creators;
public:
    virtual ~FKFactoryBase()=0;
    template<class E> void addClass(){
        const QMetaObject* m=&E::staticMetaObject;
        QString className=m->className();
        QWriteLocker lock(&_locker);
        _creators.insert(className,new FKFactoryObjectCreator<E>());
    }
    template<class E> void removeClass(){
        const QMetaObject* m=&E::staticMetaObject;
        QString className=m->className();
        QWriteLocker lock(&_locker);
        delete _creators.take(className);
        cleanClass(className);
    }
protected:
    FKFactoryBase(){}
    virtual D* create(const QString& className){
        _locker.lockForRead();
        FKAbstractFactoryObjectCreator* m=_creators.value(className,0);
        if(m){
            QObject* q=m->newInstance();
            _locker.unlock();
            if(q){
                D* d=qobject_cast<D*>(q);
                if(d)return d;
                q->deleteLater();
            }
        }
        return 0;
    }
    bool isRegistered(const QString& className){
        QReadLocker lock(&_locker);
        return _creators.contains(className);
    }

private:
    virtual void cleanClass(const QString& className){Q_UNUSED(className)}
    QReadWriteLock _locker;
};

template<class D>
FKFactoryBase<D>::~FKFactoryBase(){}


template<class D>
class FKFactory:public FKFactoryBase<D>{
public:
    FKFactory(){}
    virtual D* create(const QString& className)override{
        return FKFactoryBase<D>::create(className);
    }
};

#endif // FKFACTORY_H

