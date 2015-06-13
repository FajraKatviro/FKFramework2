#include "FKObjectPool.h"

#include "FKObject.h"

FKObjectPool::FKObjectPool(const qint32 size):_poolSize(size){

}

void FKObjectPool::clear(){
    for(auto i=_pool.constBegin();i!=_pool.constEnd();++i){
        for(auto obj=i.value().constBegin();obj!=i.value().constEnd();++obj){
            (*obj)->deleteLater();
        }
    }
    _pool.clear();
}

FKObject*FKObjectPool::take(const QString& className){
    QList<FKObject*>& pool=_pool[className];
    return pool.isEmpty() ? nullptr : pool.takeLast();
}

void FKObjectPool::add(FKObject* obj){
    const QString className(obj->metaObject()->className());
    QList<FKObject*>& pool=_pool[className];
    if(pool.size()<_poolSize){
        pool.append(obj);
    }else{
        obj->deleteLater();
    }
}
