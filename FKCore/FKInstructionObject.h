#ifndef FKINSTRUCTIONOBJECT_H
#define FKINSTRUCTIONOBJECT_H

#include <QVariant>
//#include "fkcore_global.h"

class /*FKCORESHARED_EXPORT*/ FKInstructionObject{
public:
    FKInstructionObject();
    FKInstructionObject(const qint32 reciever, const qint32 subject, const QVariant value);
    FKInstructionObject(const QList<qint32> recievers, const qint32 subject, const QVariant value);
    FKInstructionObject(const FKInstructionObject& other);
    FKInstructionObject& operator=(const FKInstructionObject& other);
    ~FKInstructionObject();
    qint32 subject()const;
    QVariant value()const;
    QList<qint32> recievers()const;
private:
    QList<qint32> _recievers;
    qint32 _subject;
    QVariant _value;

    friend QDataStream& operator<<(QDataStream& stream, const FKInstructionObject& lst);
    friend QDataStream& operator>>(QDataStream& stream, FKInstructionObject& lst);
};

QDataStream& operator<<(QDataStream& stream, const FKInstructionObject& lst);
QDataStream& operator>>(QDataStream& stream, FKInstructionObject& lst);

Q_DECLARE_METATYPE(FKInstructionObject)

#endif // FKINSTRUCTIONOBJECT_H
