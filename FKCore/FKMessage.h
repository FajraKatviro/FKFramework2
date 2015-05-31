#ifndef FKMESSAGE_H
#define FKMESSAGE_H

#include "FKSystemObject.h"

class /*FKCORESHARED_EXPORT*/ FKMessage : public FKSystemObject{
    Q_OBJECT
public:
    FKMessage();
    FKMessage(const QString& subject);
    ~FKMessage();
    QString subject()const{return _subject;}
protected:
    bool packObject(QDataStream &stream)const;
    bool loadObject(QDataStream &stream);
private:
    FKSystemObject* clone()const;
    QString _subject;
};

#endif // FKMESSAGE_H
