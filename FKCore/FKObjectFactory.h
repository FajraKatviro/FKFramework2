#ifndef FKOBJECTFACTORY
#define FKOBJECTFACTORY

#include "FKFactory.h"

class FKObject;

class FKObjectFactory:public FKFactoryBase<FKObject>{
    QMap<QString,QList<FKObject*>> _pool;
    const qint32 _poolSize;
public:
    FKObjectFactory(const qint32 poolSize);
    ~FKObjectFactory();
    virtual FKObject* create(const QString& className)override;
    void deleteObject(FKObject* target);
    void clearPool();
};

#endif // FKOBJECTFACTORY

