#ifndef FK_ORDER_OPERATOR_H
#define FK_ORDER_OPERATOR_H

#include <functional>
#include <QMap>
#include <QVariant>
#include <QDebug>

//base class allow store description types in single map
//this template provides convinient way to attach order operators to different implementation classes
template <class ImplementationClass>
struct FKDescriptionTypeBase{
    template <typename... Args>
    using checkCall=std::function<bool(ImplementationClass&,Args...)>;
    template <typename... Args>
    using getCall=std::function<QVariant(ImplementationClass&,Args...)>;
    using setCall=std::function<void(ImplementationClass&,const QVariant&)>;
    template <typename... Args>
    using requestCall=std::function<void(ImplementationClass&,Args...)>;
    using Impl=ImplementationClass;
};

//template for description respecialization
template <int r>
struct FKDescriptionType{
};

//use this macro for shorter respecialization typing
#define FKDescriptionDeclaration(id,operatorClass) template<> struct FKDescriptionType<id>:public FKDescriptionTypeBase<operatorClass>
//it is recommended to redefine description declarator macro for special implementation class in .cpp file, for example
//#define FKRequest(id) FKDescriptionDeclaration(id,MyCustomOperatorImplementationClass)

//use this macro inside opeartor implementation class for order declaration
#define FKDeclareOrder(name) template <int r,typename... Args> void name(FKDescriptionType<r>* description,Args... args)

//use this macro outside oprator implementation class for order implementation
#define FKOrder(ClassName,id,name) \
    template <class OperatorClass,int r,typename... Args> \
    struct FKOrderContainer<id,OperatorClass,r,Args...>{ \
        FKOrderContainer(OperatorClass* base,FKDescriptionType<r>* description,Args... args){base->name(description,args...);} \
    }; \
    template <int r,typename... Args> void ClassName::name(FKDescriptionType<r>* description,Args... args)

//template for order respecialization
template <int order,class OperatorClass,int r,typename... Args>
struct FKOrderContainer{
};

//helper sctructure
template<typename OrderOperator,typename... Args>
struct FKOrderCaller{
    FKOrderCaller(OrderOperator* impl,Args... arguments):_impl(impl),arguments(arguments...){}
    template<int order,int r>
    void callOrder()const{
        FKDescriptionType<r> description;
        callOrderInternal<order,r>(&description,std::index_sequence_for<Args...>{});
    }
private:
    template<int order,int r,std::size_t... index>
    void callOrderInternal(FKDescriptionType<r>* description,std::index_sequence<index...>)const{
        FKOrderContainer<order,OrderOperator,r,Args...>(_impl,description,std::get<index>(arguments)...);
    }
    OrderOperator* _impl;
    std::tuple<Args...> arguments;
};

//helper sctructure
template <int... descriptions>
struct FKAllowedDescriptions{
    template<int order,typename Caller>
    void execute(int descriptionId, const Caller& caller)const{
        deduceDescription<order,Caller,descriptions...>(descriptionId,caller);
    }
private:
    template<int order,typename Caller,int description,int... restDescriptions>
    void deduceDescription(int descriptionId,const Caller& caller)const{
        if(descriptionId==description){
            caller.template callOrder<order,description>();
        }else{
            deduceDescription<order,Caller,restDescriptions...>(descriptionId,caller);
        }
    }
    template<int order,typename Caller>
    void deduceDescription(int/* descriptionId*/,const Caller&/* caller*/)const{
        //description not found
    }
};

//helper sctructure
template <int... orders>
struct FKAllowedOrders{
    template<typename AllowedDescriptions,typename Caller>
    void execute(int orderId,int descriptionId,const Caller& caller)const{
        deduceOrder<AllowedDescriptions,Caller,orders...>(orderId,descriptionId,caller);
    }
    template<typename Caller,int r>
    void execute(int orderId,const Caller& caller)const{
        deduceOrder<Caller,r,orders...>(orderId,caller);
    }
private:
    template<typename AllowedDescriptions,typename Caller,int order, int... restOrders>
    void deduceOrder(int orderId,int descriptionId,const Caller& caller)const{
        if(orderId==order){
            AllowedDescriptions descriptions;
            descriptions.execute<order,decltype(caller)>(descriptionId,caller);
        }else{
            deduceOrder<AllowedDescriptions,Caller,restOrders...>(orderId,descriptionId,caller);
        }
    }
    template<typename AllowedDescriptions,typename Caller>
    void deduceOrder(int/* orderId*/,int/* descriptionId*/,const Caller&/* caller*/)const{
        //no order found
    }

    template<typename Caller,int r,int order, int... restOrders>
    void deduceOrder(int orderId,const Caller& caller)const{
        if(orderId==order){
            caller.template callOrder<order,r>();
        }else{
            deduceOrder<Caller,r,restOrders...>(orderId,caller);
        }
    }
    template<typename Caller,int r>
    void deduceOrder(int/* orderId*/,const Caller&/* caller*/)const{
        //no order found
    }
};

template <class Impl,typename AllowedOrders,typename AllowedDescriptions>
class FKOrderOperator:public Impl{
    static_assert(std::has_virtual_destructor<Impl>::value,"Invalid FKOrderOperator template base: implementation should have virtual destructor");

public:
    virtual ~FKOrderOperator(){

    }
    template <int r,int order,typename... Args>
    void execute(Args... args){
        FKOrderCaller<Impl,Args...> caller(this,args...);
        caller.callOrder<r,order>();
    }
    template <int r,typename... Args>
    void executeOrder(void (Impl::*orderPtr)(FKDescriptionType<r>*,Args...), Args... args){
        FKDescriptionType<r> description;
        ((*this).*orderPtr)(&description,args...);
    }
    template <int r,typename... Args>
    void executeOrder(int orderId, Args... args){
        FKOrderCaller<Impl,Args...> caller(this,args...);
        AllowedOrders orders;
        orders.execute<decltype(caller),r>(orderId,caller);
    }
    template <int order,typename... Args>
    void execute(int descriptionId, Args... args){
        FKOrderCaller<Impl,Args...> caller(this,args...);
        AllowedDescriptions descriptions;
        descriptions.execute<order,decltype(caller)>(descriptionId,caller);
    }
    template <typename... Args>
    void execute(int orderId,int descriptionId,Args... args){
        FKOrderCaller<Impl,Args...> caller(this,args...);
        AllowedOrders orders;
        orders.execute<AllowedDescriptions>(orderId,descriptionId,caller);
    }

private:
    template <int orderId,typename Arguments, int allowed, int... other>
    void deduceExecute(int id,Arguments arguments){
        if(allowed==id){
            FKDescriptionType<allowed> description;
            callOrder<orderId>(arguments,&description);
        }else{
            deduceExecute<orderId,other...>(id,arguments);
        }
    }
    template <int orderId,typename Arguments>
    void deduceExecute(int id,Arguments arguments){
        Q_UNUSED(arguments)
        qDebug()<<"not found "<<id;
    }
    template<int orderId,int r,typename Arguments>
    void callOrder(Arguments arguments,FKDescriptionType<r>* description){
        arguments.callOrder<orderId,r>(description,this);
    }
};

#endif // FK_ORDER_OPERATOR_H
