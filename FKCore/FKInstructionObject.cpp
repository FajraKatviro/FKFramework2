#include "FKInstructionObject.h"

#include <QDataStream>

//#include "FKLogger.h"

/*!
\class FKInstructionObject
\brief FKRoomEngine use this data type as command request and result report
*/


FKInstructionObject::FKInstructionObject(){

}

FKInstructionObject::FKInstructionObject(const qint32 subject, const QVariant value):
    FKInstructionObject(QList<qint32>(),subject,value){

}

FKInstructionObject::FKInstructionObject(const qint32 reciever, const qint32 subject, const QVariant value):
    FKInstructionObject(QList<qint32>()<<reciever,subject,value){

}

FKInstructionObject::FKInstructionObject(const QList<qint32> recievers, const qint32 subject, const QVariant value):
    _recievers(recievers),_subject(subject),_value(value){

}

FKInstructionObject::FKInstructionObject(const FKInstructionObject& other):
    _recievers(other._recievers),_subject(other._subject),_value(other._value){

}

FKInstructionObject& FKInstructionObject::operator=(const FKInstructionObject& other){
    _recievers=other._recievers;
    _subject=other._subject;
    _value=other._value;
    return *this;
}

FKInstructionObject::~FKInstructionObject(){

}

qint32 FKInstructionObject::subject() const{
    return _subject;
}

QVariant FKInstructionObject::value() const{
    return _value;
}

QList<qint32> FKInstructionObject::recievers() const{
    return _recievers;
}

qint32 FKInstructionObject::reciever() const{
    return _recievers.isEmpty() ? 0 : _recievers.first();
}

QDataStream& operator<<(QDataStream& stream, const FKInstructionObject& lst){
    stream<<lst._recievers<<lst._subject<<lst._value;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, FKInstructionObject& lst){
    stream>>lst._recievers>>lst._subject>>lst._value;
    return stream;
}
