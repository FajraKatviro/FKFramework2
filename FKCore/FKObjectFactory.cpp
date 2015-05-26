#include "FKObjectFactory.h"
#include "FKObject.h"

FKObjectFactory::FKObjectFactory(const qint32 poolSize)
    :FKFactoryBase<FKObject>()
    ,_poolSize(poolSize){}

FKObjectFactory::~FKObjectFactory(){
    clearPool();
}

FKObject* FKObjectFactory::create(const QString& className){
    QList<FKObject*>& pool=_pool[className];
    return pool.isEmpty() ? FKFactoryBase<FKObject>::create(className) :
                            pool.takeLast();
}

void FKObjectFactory::deleteObject(FKObject* target){
    QList<FKObject*>& pool=_pool[target->metaObject()->className()];
    if(pool.size()<_poolSize){
        pool.append(target);
    }else{
        target->deleteLater();
    }
}

void FKObjectFactory::clearPool(){
    for(auto i=_pool.constBegin();i!=_pool.constEnd();++i){
        for(auto obj=i.value().constBegin();obj!=i.value().constEnd();++obj){
            (*obj)->deleteLater();
        }
    }
    _pool.clear();
}
