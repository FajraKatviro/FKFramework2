#include "FKBasicEvent.h"

#include "FKLogger.h"

/*!
\class FKBasicEvent
\brief Class extends FKMessage by \i value data. This objects contains commands for infrastructires and consumed by connection managers
*/

/*!
 * \brief Constructs empty basic event
 */

FKBasicEvent::FKBasicEvent():FKMessage(),_value(){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Constructs basic event \i subject with \i value
 */

FKBasicEvent::FKBasicEvent(const QString& subject, const QVariant value):FKMessage(subject),_value(value){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Destructs event object
 */

FKBasicEvent::~FKBasicEvent(){
    FK_DBEGIN
    FK_DEND
}

/*!
 * \fn QVariant FKBasicEvent::value()const
 * \brief Returns event's value
 */

/*!
 * \brief This is reimplemented function. Write \i value to stream
 */

bool FKBasicEvent::packObject(QDataStream &stream)const{
    if(FKMessage::packObject(stream)){
        stream<<_value;
        return true;
    }
    return false;
}

/*!
 * \brief This is reimplemented function. Read \i value from stream
 */

bool FKBasicEvent::loadObject(QDataStream &stream){
    if(FKMessage::loadObject(stream)){
        stream>>_value;
        return true;
    }
    return false;
}

/*!
 * \brief This is reimplemented function
 */

FKSystemObject* FKBasicEvent::clone() const{
    return new FKBasicEvent(subject(),_value);
}
