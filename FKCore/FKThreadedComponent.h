#ifndef FKTHREADEDCOMPONENT_H
#define FKTHREADEDCOMPONENT_H

#include <QObject>
#include <QThread>
#include <QSharedPointer>

struct FKAbstractFactoryObjectCreator;

class FKThreadedComponent:public QObject{
    Q_OBJECT
public:
    FKThreadedComponent(QObject *parent = 0);
    ~FKThreadedComponent();
    virtual void startComponent(){}
    virtual void stopComponent();
    bool isRunning()const;
    void setComponentFactory(FKAbstractFactoryObjectCreator* creator);
protected:
    void startComponent(QObject* object);
    QObject* component()const;
    template<typename ReturnType>
    bool getValue(ReturnType& returnValue,const char* methodName){
        return QMetaObject::invokeMethod(_componentObject,methodName,Qt::BlockingQueuedConnection,Q_RETURN_ARG(ReturnType,returnValue));
    }
    template<typename ReturnType,typename ArgType>
    bool getValue(ReturnType& returnValue,const char* methodName,const ArgType& argValue){
        return QMetaObject::invokeMethod(_componentObject,methodName,Qt::BlockingQueuedConnection,Q_RETURN_ARG(ReturnType,returnValue),Q_ARG(ArgType,argValue));
    }
    bool callMethod(const char* methodName){
        return QMetaObject::invokeMethod(_componentObject,methodName,Qt::QueuedConnection);
    }
    template<typename ArgType>
    bool callMethod(const char* methodName,const ArgType& argValue){
        return QMetaObject::invokeMethod(_componentObject,methodName,Qt::QueuedConnection,Q_ARG(ArgType,argValue));
    }
protected:
    inline FKAbstractFactoryObjectCreator* componentFactory(){return _componentFactory;}
private:
    FKAbstractFactoryObjectCreator* _componentFactory=nullptr;
    QObject* _componentObject=nullptr;
    QThread _componentThread;
};

#endif // FKTHREADEDCOMPONENT_H
