#ifndef FKREQUESTS_H
#define FKREQUESTS_H

#include <functional>
#include <QMap>

//base class allow store request types in single map
//this template provides convinient way to attach requests to different operator classes
template <class OperatorClass>
struct FKRequestTypeBase{
    template <typename... Args>
    using checkCall=std::function<bool(OperatorClass&,Args...)>;
    template <typename... Args>
    using requestCall=std::function<int(OperatorClass&,Args...)>;
    using replyCall=std::function<void(OperatorClass&,int)>;
};

//template for respecialization
template <int r>
struct FKRequestType{
};

//use this macro for shorter respecialization typing
#define FKRequestDeclaration(id,operatorClass) template<> struct FKRequestType<id>:public FKRequestTypeBase<operatorClass>

//it is recommended to redefine request declarator macro for special operator class in .cpp file, for example
//#define FKRequest(id) FKRequestDeclaration(id,MyCustomOperator)

struct FKRequestInstance{

};

template <class Impl>
class FKRequestOperator:public Impl{
    static_assert(std::has_virtual_destructor<Impl>::value,"Invalid FKRequestOperatorBasic template base: implementation should have virtual destructor");
public:
    virtual ~FKRequestOperator(){
        qDeleteAll(activeRequests);
        qDeleteAll(allowedRequests);
    }
    template <int r,typename... Args>
    void doRequest(Args... args){
        FKRequestType<r>* request=static_cast<FKRequestType<r>*>(allowedRequests.value(r,nullptr));
        if(!request)return;
        if(request->check(*this,args...)){
            activeRequests.insert(r,new FKRequestInstance());
            //request->request(*this,args...);
        }
    }
    template <int r>
    void allowRequest(){
        allowedRequests.insert(r,new FKRequestType<r>());
    }
private:
    QMap<int,FKRequestTypeBase<Impl>* > allowedRequests;
    QMap<int,FKRequestInstance*> activeRequests;
};

#endif // FKREQUESTS_H
