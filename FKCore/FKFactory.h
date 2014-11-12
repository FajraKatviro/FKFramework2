#ifndef FKFACTORY_H
#define FKFACTORY_H

#include <QObject>
#include <QMetaObject>
#include <QMap>

#include "fkcore_global.h"

template<class D>
class /*FKCORESHARED_EXPORT*/ FKFactory{
    QMap<QString,const QMetaObject*> _metaObjects;
public:
    template<class E> void addClass(){
        const QMetaObject* m=&E::staticMetaObject;
        QString className=m->className();
        _metaObjects.insert(className,m);
    }

    D* create(const QString& className)const{
        const QMetaObject* m=_metaObjects.value(className,0);
        if(m){
            QObject* q=m->newInstance();
            if(q){
                D* d=qobject_cast<D*>(q);
                if(d)return d;
                q->deleteLater();
            }
        }
        return 0;
    }
};

#endif // FKFACTORY_H
