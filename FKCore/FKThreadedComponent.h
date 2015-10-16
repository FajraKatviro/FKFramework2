#ifndef FKTHREADEDCOMPONENT_H
#define FKTHREADEDCOMPONENT_H

#include <QObject>
#include <QThread>
#include <QSharedPointer>

#include "fkcore_global.h"

struct FKAbstractFactoryObjectCreator;

class FKCORESHARED_EXPORT FKThreadedComponent:public QObject{
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
    bool getValue(ReturnType& returnValue,const char* returnType,const char* methodName)const{
        return QMetaObject::invokeMethod(_componentObject,methodName,Qt::BlockingQueuedConnection,QReturnArgument<ReturnType>(returnType,returnValue));
    }
    template<typename ReturnType,typename ArgType>
    bool getValue(ReturnType& returnValue, const char* returnType,const char* methodName,const ArgType& argValue, const char* argType)const{
        return QMetaObject::invokeMethod(_componentObject,methodName,Qt::BlockingQueuedConnection,QReturnArgument<ReturnType>(returnType,returnValue),QArgument<ArgType>(argType,argValue));
    }
    bool callMethod(const char* methodName)const{
        return QMetaObject::invokeMethod(_componentObject,methodName,Qt::QueuedConnection);
    }
    template<typename ArgType>
    bool callMethod(const char* methodName,const ArgType& argValue, const char* argType)const{
        return QMetaObject::invokeMethod(_componentObject,methodName,Qt::QueuedConnection,QArgument<ArgType>(argType,argValue));
    }
    template<typename ArgType,typename Arg2Type>
    bool callMethod(const char* methodName,const ArgType& argValue, const char* argType,const Arg2Type& arg2Value, const char* arg2Type)const{
        return QMetaObject::invokeMethod(_componentObject,methodName,Qt::QueuedConnection,QArgument<ArgType>(argType,argValue),QArgument<Arg2Type>(arg2Type,arg2Value));
    }
protected:
    inline FKAbstractFactoryObjectCreator* componentFactory(){return _componentFactory;}
protected slots:
    virtual void componentThreadQuit();
private:
    FKAbstractFactoryObjectCreator* _componentFactory=nullptr;
    QObject* _componentObject=nullptr;
    QThread _componentThread;
    bool _stopRequested=false;
};

#define FK_THREAD_GETTER(RetType,retValue,methodName)                       getValue(retValue,#RetType,#methodName)
#define FK_THREAD_GETTER_ARG(RetType,retValue,methodName,ArgType,argValue)  getValue(retValue,#RetType,#methodName,argValue,#ArgType)
#define FK_THREAD_CALL(methodName)                                          callMethod(#methodName)
#define FK_THREAD_CALL_ARG(methodName,ArgType,argValue)                     callMethod(#methodName,argValue,#ArgType)

#endif // FKTHREADEDCOMPONENT_H
