#ifndef FKEVENT_H
#define FKEVENT_H

#include "FKSystemObject.h"

#include <QVariant>

class FKMessage;

class /*FKCORESHARED_EXPORT*/ FKEvent:public FKSystemObject{
    Q_OBJECT
public:
    ~FKEvent();
    qint32 object()const{return _object;}
    QString subject()const{return _subject;}
    QVariant value()const{return _value;}
    QList<qint32> recievers()const{return _recievers;}
    qint32 client()const{return _client;}
    qint32 emitter()const{return _emitter;}
    bool isPropertyNotifier()const{return _isPropertyNotifier;}
    static FKMessage* eventError(FKEvent* errorSource, const QString& reason);
    static FKEvent* makeAction(const qint32 object,const QString& subject,const QVariant& value,const qint32 client,const qint32 emitter);
    static FKEvent* makeEvent(const qint32 object,const QString& subject,const QVariant& value,const QList<qint32>& recievers,const bool propertyChangeNotifier);
    static FKEvent* makeEvent(const qint32 object,const QString& subject,const QVariant& value,const qint32 reciever,const bool propertyChangeNotifier);
private:
    FKEvent();
    FKEvent(const FKEvent* other);
    FKEvent(const qint32 object,const QString& subject,const QVariant& value,const qint32 client,const qint32 emitter);
    FKEvent(const qint32 object,const QString& subject,const QVariant& value,const QList<qint32>& recievers,const bool propertyChangeNotifier);
    bool packObject(QDataStream &stream)const;
    bool loadObject(QDataStream &stream);
    FKSystemObject* clone() const;
    qint32 _object;
    QString _subject;
    QVariant _value;
    QList<qint32> _recievers;
    qint32 _client;
    qint32 _emitter;
    bool _isPropertyNotifier;

    friend class FKFactoryObjectCreator<FKEvent>;
};

#endif // FKEVENT_H
