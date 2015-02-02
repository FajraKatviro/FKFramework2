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
    template<class D,class D1,class...Other>
    static void allowConstructing(){
        allowConstructing<D>();
        allowConstructing<D1,Other...>();
    }
    template<class D>
    static void forbidConstructing(){
        _factory.removeClass<D>();
    }
    template<class D,class D1,class...Other>
    static void forbidConstructing(){
        forbidConstructing<D>();
        forbidConstructing<D1,Other...>();
    }
    virtual FKSystemObject* clone()const=0;
private:
    virtual bool packObject(QDataStream& stream)const=0;
    virtual bool loadObject(QDataStream& stream)=0;
    static FKFactory<FKSystemObject> _factory;
    static const QString objectIdentifier;
};

#endif // FKSYSTEMOBJECT_H
