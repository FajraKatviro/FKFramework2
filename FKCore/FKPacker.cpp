#include "FKPacker.h"
#include <QByteArray>
#include "FKSystemObject.h"
#include <QDataStream>

QByteArray FKPacker::pack(QString messageType, FKSystemObject* obj){
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_3);
    stream<<messageType;
    obj->pack(stream);
    return arr;
}

QString FKPacker::unpack(QByteArray arr, FKSystemObject* &container){
    if(container)return QString("Not empty container provided to packer");
    QDataStream stream(&arr, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_5_3);
    QString type, className;
    stream>>type;
    if(type==event){
        className=QString("FKEvent");
    }else if(type==basicEvent || type==guestEvent){
        className=QString("FKBasicEvent");
    }else if(type==message){
        className=QString("FKMessage");
    }else if(type==updateData){
        className=QString("FKUpdateData");
    }else{
        return QString("Invalid event type recieved by packer: ")+type;
    }
    container=FKSystemObject::create(className);
    if(!container)return QString("Packer unable create object of type ")+className;
    if(!container->load(stream)){
        container->deleteLater();
        container=0;
        return QString("Packer unable load object of type ")+className;
    }
    return type;
}
