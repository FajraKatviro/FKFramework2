#ifndef FKSYSTEMOBJECT_H
#define FKSYSTEMOBJECT_H

#include "FKFactory.h"

#include "fkcore_global.h"


#define FK_OBJECT_REGISTRATOR(Operation) \
template<class D,class D1,class...Other>\
static void allow##Operation(){\
    allow##Operation<D>();\
    allow##Operation<D1,Other...>();\
}\
template<class D,class D1,class...Other>\
static void forbid##Operation(){\
    forbid##Operation<D>();\
    forbid##Operation<D1,Other...>();\
}

class /*FKCORESHARED_EXPORT*/ FKSystemObject:public QObject{
    Q_OBJECT
public:
    FKSystemObject();
    ~FKSystemObject();
    bool pack(QDataStream& stream)const;
    bool load(QDataStream& stream);
    static FKSystemObject* create(const QString& className);
    virtual FKSystemObject* clone()const=0;
    FK_OBJECT_REGISTRATOR(Constructing)
    template<class D>
    static void allowConstructing(){
        _factory.addClass<D>();
    }
    template<class D>
    static void forbidConstructing(){
        _factory.removeClass<D>();
    }
private:
    virtual bool packObject(QDataStream& stream)const=0;
    virtual bool loadObject(QDataStream& stream)=0;
    static FKFactory<FKSystemObject> _factory;
    static const QString objectIdentifier;
};

#endif // FKSYSTEMOBJECT_H
