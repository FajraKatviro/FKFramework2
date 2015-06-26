#include "FKEventObject.h"

#include "FKMessage.h"

#include "FKLogger.h"

/*!
\class FKEventObject
\brief This is main event-delivery container. Instancies used for event, action and property change notification delivery.
*/

/*!
 * \brief Destructs event object
 */

FKEventObject::~FKEventObject(){
    FK_DBEGIN
    FK_DEND
}

/*!
 * \fn qint32 FKEventObject::object()const
 * \brief Returns target object's id
 */

/*!
 * \fn QString FKEventObject::subject()const
 * \brief Returns event's type
 */

/*!
 * \fn QVariant FKEventObject::value()const
 * \brief Return event's value
 */

/*!
 * \fn QList<qint32> FKEventObject::recievers()const
 * \brief Return list of event recievers id. This is server-side function
 */

/*!
 * \fn qint32 FKEventObject::client()const
 * \brief Return sender client id
 */

/*!
 * \fn qint32 FKEventObject::emitter()const
 * \brief Return emitter's id. Emitter is a FKObject that initiated event creation
 */

/*!
 * \fn bool FKEventObject::isPropertyNotifier()const
 * \brief Return true if event notifies property change and should be passed to property change handler
 */

/*!
 * \brief Constructs message for error event description
 */

FKMessage* FKEventObject::eventError(FKEventObject* errorSource,const QString& reason){
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

FKEventObject* FKEventObject::makeAction(const qint32 object,const qint32 subject,const QVariant& value,const qint32 client,const qint32 emitter){
    FK_MLOGV(QString(tr("Action %1 for target %2 by %3 emitter from %4 client created").arg(subject).arg(object).arg(emitter).arg(client)),value)
    return new FKEventObject(object,subject,value,client,emitter);
}

/*!
 * \brief Creates \i subject event targeted \i object for multiple recievers
 */

FKEventObject* FKEventObject::makeEvent(const qint32 object,const qint32 subject,const QVariant& value,const QList<qint32>& recievers,const bool propertyChangeNotifier){
    FK_MLOGV(QString(tr("Event %1 for target %2 for %3 recivers created. Property change: %4").arg(subject).arg(object).arg(recievers.count()).arg(propertyChangeNotifier)),value)
    return new FKEventObject(object,subject,value,recievers,propertyChangeNotifier);
}

/*!
 * \brief Creates \i subject event targeted \i object for single reciever
 */

FKEventObject* FKEventObject::makeEvent(const qint32 object,const qint32 subject,const QVariant& value,const qint32 reciever,const bool propertyChangeNotifier){
    FK_MLOGV(QString(tr("Event %1 for target %2 for %3 id reciver created. Property change: %4").arg(subject).arg(object).arg(reciever).arg(propertyChangeNotifier)),value)
    QList<qint32> recievers;
    recievers.append(reciever);
    return new FKEventObject(object,subject,value,recievers,propertyChangeNotifier);
}

FKEventObject::FKEventObject():FKSystemObject(),_object(-1),_subject(0),_client(-1),_emitter(-1),_isPropertyNotifier(false){
    FK_CBEGIN
    FK_CEND
}

FKEventObject::FKEventObject(const FKEventObject* other):FKSystemObject(),
        _object(other->_object),_subject(other->_subject),_value(other->_value),
        _client(other->_client),_emitter(other->_emitter),_isPropertyNotifier(other->_isPropertyNotifier){
    FK_CBEGIN
    FK_CEND
}

FKEventObject::FKEventObject(const qint32 object,const qint32 subject,const QVariant& value,const qint32 client,const qint32 emitter):
        FKSystemObject(),_object(object),_subject(subject),_value(value),_client(client),_emitter(emitter),_isPropertyNotifier(false){
    FK_CBEGIN
    FK_CEND
}

FKEventObject::FKEventObject(const qint32 object,const qint32 subject,const QVariant& value,const QList<qint32>& recievers,const bool propertyChangeNotifier):
        FKSystemObject(),_object(object),_subject(subject),_value(value),_recievers(recievers),_client(-1),_emitter(-1),_isPropertyNotifier(propertyChangeNotifier){
    FK_CBEGIN
    FK_CEND
}

bool FKEventObject::packObject(QDataStream &stream)const{
    stream<<_object<<
            _subject<<
            _value<<
            _client<<
            _emitter<<
            _isPropertyNotifier;
    return true;
}

bool FKEventObject::loadObject(QDataStream &stream){
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

FKSystemObject* FKEventObject::clone() const{
    return new FKEventObject(this);
}
