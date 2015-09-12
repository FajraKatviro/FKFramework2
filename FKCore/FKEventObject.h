#ifndef FKEVENTOBJECT_H
#define FKEVENTOBJECT_H

#include "FKSystemObject.h"

#include <QVariant>

class FKMessage;

class /*FKCORESHARED_EXPORT*/ FKEventObject:public FKSystemObject{
    Q_OBJECT
public:
    ~FKEventObject();
    qint32 object()const{return _object;}
    qint32 subject()const{return _subject;}
    QVariant value()const{return _value;}
    QList<qint32> recievers()const{return _recievers;}
    qint32 client()const{return _client;}
    qint32 emitter()const{return _emitter;}
    bool isPropertyNotifier()const{return _isPropertyNotifier;}

    void setRecievers(const QList<qint32>& recievers);

    static FKMessage* eventError(FKEventObject* errorSource, const QString& reason);
    static FKEventObject* makeAction(const qint32 object,const qint32 subject,const QVariant& value,const qint32 client,const qint32 emitter);
    static FKEventObject* makeEvent(const qint32 object,const qint32 subject,const QVariant& value,const QList<qint32>& recievers,const bool propertyChangeNotifier);
    static FKEventObject* makeEvent(const qint32 object,const qint32 subject,const QVariant& value,const qint32 reciever,const bool propertyChangeNotifier);
private:
    FKEventObject();
    FKEventObject(const FKEventObject* other);
    FKEventObject(const qint32 object,const qint32 subject,const QVariant& value,const qint32 client,const qint32 emitter);
    FKEventObject(const qint32 object,const qint32 subject,const QVariant& value,const QList<qint32>& recievers,const bool propertyChangeNotifier);
    bool packObject(QDataStream &stream)const;
    bool loadObject(QDataStream &stream);
    FKSystemObject* clone() const;
    qint32 _object;
    qint32 _subject;
    QVariant _value;
    QList<qint32> _recievers;
    qint32 _client;
    qint32 _emitter;
    bool _isPropertyNotifier;

    friend class FKFactoryObjectCreator<FKEventObject>;
};

#endif // FKEVENTOBJECT_H
