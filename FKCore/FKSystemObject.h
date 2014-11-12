#ifndef FKSYSTEMOBJECT_H
#define FKSYSTEMOBJECT_H

#include "FKFactory.h"

#include "fkcore_global.h"

class /*FKCORESHARED_EXPORT*/ FKSystemObject:public QObject{
    Q_OBJECT
public:
    FKSystemObject();
    ~FKSystemObject();
    bool pack(QDataStream& stream)const;
    bool load(QDataStream& stream);
    static FKSystemObject* create(const QString& className);
    template<class D>
    static void allowConstructing(){
        _factory.addClass<D>();
    }
    virtual FKSystemObject* clone()const=0;
private:
    virtual bool packObject(QDataStream& stream)const=0;
    virtual bool loadObject(QDataStream& stream)=0;
    static FKFactory<FKSystemObject> _factory;
    static const QString objectIdentifier;
};

#endif // FKSYSTEMOBJECT_H
