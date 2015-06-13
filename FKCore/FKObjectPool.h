#ifndef FKOBJECTPOOL_H
#define FKOBJECTPOOL_H

#include <QMap>

class FKObject;

class FKObjectPool{
public:
    FKObjectPool(const qint32 size);
    ~FKObjectPool(){}
    void clear();
    FKObject* take(const QString& className);
    void add(FKObject* obj);
private:
    QMap<QString,QList<FKObject*>> _pool;
    const qint32 _poolSize;
};

#endif // FKOBJECTPOOL_H
