#ifndef FK_ORDER_OPERATORS_H
#define FK_ORDER_OPERATORS_H

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
    using replyCall=std::function<void(ImplementationClass&,const QVariant&)>;
    using Impl=ImplementationClass;
};

//template for respecialization
template <int r>
struct FKDescriptionType{
};

//use this macro for shorter respecialization typing
#define FKDescriptionDeclaration(id,operatorClass) template<> struct FKDescriptionType<id>:public FKDescriptionTypeBase<operatorClass>
//it is recommended to redefine description declarator macro for special implementation class in .cpp file, for example
//#define FKRequest(id) FKDescriptionDeclaration(id,MyCustomOperatorImplementationClass)

//use this macro inside opeartor implementation class for shorthand order declaration
#define FKOrder(name) template <int r,typename... Args> void name(FKDescriptionType<r>* description,Args... args)

template <int order,class OperatorClass,int r,typename... Args>
struct FKOrderContainer{
};

//template <class OperatorClass,int r,typename... Args>
//struct FKOrderContainer<0>{
//    FKOrderContainer(OperatorClass* base,FKDescriptionType<r>* description,Args... args){base->customBehaviour(description,args);}
//};

template <class OperatorClass,int r,typename... Args>
struct FKOrderContainer<0,OperatorClass,r,Args...>{
    FKOrderContainer(OperatorClass* base,FKDescriptionType<r>* description,Args... args){base->customBehaviour(description,args...);}
};

template <class Impl,int... AllowedDescriptions>
class FKOrderOperator:public Impl{
    static_assert(std::has_virtual_destructor<Impl>::value,"Invalid FKOrderOperator template base: implementation should have virtual destructor");
    template<typename... Args>
    struct CallArguments{
        CallArguments(const std::tuple<Args...>& arguments):arguments(arguments){}
        std::tuple<Args...> arguments;
        template<int orderId,int r>
        void callOrder(FKDescriptionType<r>* description,FKOrderOperator<Impl,AllowedDescriptions...>* impl){
            callOrder<orderId,r>(description,impl,std::index_sequence_for<Args...>{});
        }
    private:
        template<int orderId,int r,std::size_t... index>
        void callOrder(FKDescriptionType<r>* description,FKOrderOperator<Impl,AllowedDescriptions...>* impl,std::index_sequence<index...>){
            FKOrderContainer<orderId,FKOrderOperator<Impl,AllowedDescriptions...>,r,Args...>(impl,description,std::get<index>(arguments)...);
        }
    };
public:
    virtual ~FKOrderOperator(){

    }
    template <int r,typename... Args>
    void execute(void (FKOrderOperator<Impl,AllowedDescriptions...>::*order)(FKDescriptionType<r>*,Args...), Args... args){
        FKDescriptionType<r> description;
        ((*this).*order)(&description,args...);
    }
    template <int orderId,typename... Args>
    void execute(int id,Args... args){
        CallArguments<Args...> arguments(std::tuple<Args...>(args...));
        deduceExecute<orderId,decltype(arguments),AllowedDescriptions...>(id,arguments);
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

#endif // FK_ORDER_OPERATORS_H
