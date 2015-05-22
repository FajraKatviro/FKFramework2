#ifndef FKOBJECTFACTORY
#define FKOBJECTFACTORY

#include "FKFactory.h"
#include "FKObject.h"

class FKObjectFactory:public FKFactoryBase<FKObject>{
    QMap<QString,QList<FKObject*>> _pool;
    const qint32 _poolSize;
public:
    FKObjectFactory(const qint32 poolSize):_poolSize(poolSize){}
    ~FKObjectFactory(){
        clearPool();
    }
    virtual FKObject* create(const QString& className)override{
        QList<FKObject*>& pool=_pool[className];
        return pool.isEmpty() ? FKFactoryBase<FKObject>::create(className) :
                                pool.takeLast();
    }
    void deleteObject(FKObject* target){
        QList<FKObject*>& pool=_pool[target->metaObject()->className()];
        if(pool.size()<_poolSize){
            pool.append(target);
        }else{
            target->deleteLater();
        }
    }
    void clearPool(){
        for(auto i=_pool.constBegin();i!=_pool.constEnd();++i){
            for(auto obj=i.value().constBegin();obj!=i.value().constEnd();++obj){
                obj->deleteLater();
            }
        }
    }
};

#endif // FKOBJECTFACTORY

