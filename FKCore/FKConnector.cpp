#include "FKConnector.h"

#include "FKLogger.h"

/*!
\class FKConnector
\brief This abstract class provides interface to transfer different messages between applications and infrastructures
*/

/*!
 * \brief Creates object
 */

FKConnector::FKConnector(QObject *parent):QObject(parent),_status(FKConnector::Disconnected){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Disconnect and deletes connector object
 */

FKConnector::~FKConnector(){
    FK_DBEGIN
    setStatus(Disconnected);
    FK_DEND
}

/*!
 * \fn FKConnector::Status FKConnector::status()const
 * \brief Get connector status. Default status is disconnected.
 */

/*!
 * \brief Set connector status
 */

void FKConnector::setStatus(FKConnector::Status status){
    if(_status!=status){
        _status=status;
        emit statusChanged(_status);
    }
}

/*!
 * \fn void FKConnector::statusChanged(FKConnector::Status status)
 * \brief This signal emitted whenever connector status is changed
 */

/*!
 * \fn void FKConnector::sendMessage(const QString& msgType, FKSystemObject* msg)
 * \brief Send \i msg of type \i msgType
 */

/*!
 * \fn void FKConnector::sendMessage(QByteArray data)
 * \brief Send msg previously packed to byte array
 */

/*!
 * \fn void FKConnector::gotMessage(const QString& msgType, FKSystemObject* msg)
 * \brief Signal emitted when message \i msg of type \i msgType recieved. Message instance created by connector.
 */
