#include "FKEvent.h"

#include "FKMessage.h"

#include "FKLogger.h"

/*!
\class FKEvent
\brief This is main event-delivery container. Instancies used for event, action and property change notification delivery.
*/

/*!
 * \brief Destructs event object
 */

FKEvent::~FKEvent(){
    FK_DBEGIN
    FK_DEND
}

/*!
 * \fn qint32 FKEvent::object()const
 * \brief Returns target object's id
 */

/*!
 * \fn QString FKEvent::subject()const
 * \brief Returns event's type
 */

/*!
 * \fn QVariant FKEvent::value()const
 * \brief Return event's value
 */

/*!
 * \fn QList<qint32> FKEvent::recievers()const
 * \brief Return list of event recievers id. This is server-side function
 */

/*!
 * \fn qint32 FKEvent::client()const
 * \brief Return sender client id
 */

/*!
 * \fn qint32 FKEvent::emitter()const
 * \brief Return emitter's id. Emitter is a FKObject that initiated event creation
 */

/*!
 * \fn bool FKEvent::isPropertyNotifier()const
 * \brief Return true if event notifies property change and should be passed to property change handler
 */

/*!
 * \brief Constructs message for error event description
 */

FKMessage* FKEvent::eventError(FKEvent* errorSource,const QString& reason){
    QString msg(tr("Unable handle %1 event for target %2 emitted by %3 from client %4: %5")
                .arg(errorSource->subject())
                .arg(errorSource->object())
                .arg(errorSource->emitter())
                .arg(errorSource->client())
                .arg(reason));
    FK_MLOG(msg)
    return new FKMessage(msg);
}

/*!
 * \brief Creates event object containing data for \i subject action targeting \i object requested by \i emitter from \i client side with \i value
 */

FKEvent* FKEvent::makeAction(const qint32 object,const QString& subject,const QVariant& value,const qint32 client,const qint32 emitter){
    FK_MLOGV(QString(tr("Action %1 for target %2 by %3 emitter from %4 client created").arg(subject).arg(object).arg(emitter).arg(client)),value)
    return new FKEvent(object,subject,value,client,emitter);
}

/*!
 * \brief Creates \i subject event targeted \i object for multiple recievers
 */

FKEvent* FKEvent::makeEvent(const qint32 object,const QString& subject,const QVariant& value,const QList<qint32>& recievers,const bool propertyChangeNotifier){
    FK_MLOGV(QString(tr("Event %1 for target %2 for %3 recivers created. Property change: %4").arg(subject).arg(object).arg(recievers.count()).arg(propertyChangeNotifier)),value)
    return new FKEvent(object,subject,value,recievers,propertyChangeNotifier);
}

/*!
 * \brief Creates \i subject event targeted \i object for single reciever
 */

FKEvent* FKEvent::makeEvent(const qint32 object,const QString& subject,const QVariant& value,const qint32 reciever,const bool propertyChangeNotifier){
    FK_MLOGV(QString(tr("Event %1 for target %2 for %3 id reciver created. Property change: %4").arg(subject).arg(object).arg(reciever).arg(propertyChangeNotifier)),value)
    QList<qint32> recievers;
    recievers.append(reciever);
    return new FKEvent(object,subject,value,recievers,propertyChangeNotifier);
}

FKEvent::FKEvent():FKSystemObject(),_object(-1),_client(-1),_emitter(-1),_isPropertyNotifier(false){
    FK_CBEGIN
    FK_CEND
}

FKEvent::FKEvent(const FKEvent* other):FKSystemObject(),
        _object(other->_object),_subject(other->_subject),_value(other->_value),
        _client(other->_client),_emitter(other->_emitter),_isPropertyNotifier(other->_isPropertyNotifier){
    FK_CBEGIN
    FK_CEND
}

FKEvent::FKEvent(const qint32 object,const QString& subject,const QVariant& value,const qint32 client,const qint32 emitter):
        FKSystemObject(),_object(object),_subject(subject),_value(value),_client(client),_emitter(emitter),_isPropertyNotifier(false){
    FK_CBEGIN
    FK_CEND
}

FKEvent::FKEvent(const qint32 object,const QString& subject,const QVariant& value,const QList<qint32>& recievers,const bool propertyChangeNotifier):
        FKSystemObject(),_object(object),_subject(subject),_value(value),_recievers(recievers),_client(-1),_emitter(-1),_isPropertyNotifier(propertyChangeNotifier){
    FK_CBEGIN
    FK_CEND
}

bool FKEvent::packObject(QDataStream &stream)const{
    stream<<_object<<
            _subject<<
            _value<<
            _client<<
            _emitter<<
            _isPropertyNotifier;
    return true;
}

bool FKEvent::loadObject(QDataStream &stream){
    stream>>_object>>
            _subject>>
            _value>>
            _client>>
            _emitter>>
            _isPropertyNotifier;
    return true;
}

/*!
 * \brief This is reimplemented function. Creates copy of event with only deliveryble content.
 */

FKSystemObject* FKEvent::clone() const{
    return new FKEvent(this);
}
