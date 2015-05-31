#ifndef FKBASICEVENT_H
#define FKBASICEVENT_H

#include "FKMessage.h"

#include "fkcore_global.h"

#include <QVariant>

class /*FKCORESHARED_EXPORT*/ FKBasicEvent:public FKMessage{
    Q_OBJECT
public:
    FKBasicEvent();
    FKBasicEvent(const QString& subject, const QVariant value=QVariant());
    ~FKBasicEvent();
    QVariant value()const{return _value;}
protected:
    bool packObject(QDataStream &stream) const;
    bool loadObject(QDataStream &stream);
private:
    FKSystemObject* clone()const;
    QVariant _value;
};

#endif // FKBASICEVENT_H
