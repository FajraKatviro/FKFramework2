#include "FKMessage.h"

#include "FKLogger.h"

/*!
\class FKMessage
\brief Class provides simple message container
*/

/*!
 * \brief Default constructor. Create empty message object
 */

FKMessage::FKMessage():FKSystemObject(),_subject(){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Create message containing \i subject
 */

FKMessage::FKMessage(const QString& subject):FKSystemObject(),_subject(subject){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Destructs message object
 */

FKMessage::~FKMessage(){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \fn QString FKMessage::subject()const
 * \brief Returns message text
 */

/*!
 * \brief This is reimplemented function. Pack subject to stream.
 */

bool FKMessage::packObject(QDataStream &stream) const{
    stream<<_subject;
    return true;
}

/*!
 * \brief This is reimplemented function. Read subject from stream.
 */

bool FKMessage::loadObject(QDataStream &stream){
    stream>>_subject;
    return true;
}

/*!
 * \brief This is reimplemented function.
 */

FKSystemObject* FKMessage::clone() const{
    return new FKMessage(_subject);
}
