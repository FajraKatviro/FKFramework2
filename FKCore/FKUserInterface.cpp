#include "FKUserInterface.h"

#include "FKLogger.h"

/*!
\class FKUserInterface
\brief Object represents abstract user interface that pass user commands to specified core and show core's response
*/

/*!
 * \brief Constructs empty interface object
 */

FKUserInterface::FKUserInterface(QObject *parent):QObject(parent){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Deletes interface object
 */

FKUserInterface::~FKUserInterface(){
    FK_DBEGIN
    FK_DEND
}

/*!
 * \brief Check arg is valid and send createUserRequested signal
 */

void FKUserInterface::requestCreateUser(const QString& arg){
    auto userName=arg.trimmed();
    if(!userName.isEmpty()){
        emit createUserRequested(userName);
    }else{
        showMessage(QString(tr("Unable request user creation: invalid user name")));
    }
}

/*!
 * \brief Check arg is valid and send deleteUserRequested signal
 */

void FKUserInterface::requestDeleteUser(QString arg){
    auto userName=arg.trimmed();
    if(!userName.isEmpty()){
        emit deleteUserRequested(userName);
    }else{
        showMessage(QString(tr("Unable request user deletion: invalid user name")));
    }
}

/*!
 * \fn void FKUserInterface::show()
 * \brief Pure virtual function. Make initial setup and run userinterface
 */
/*!
 * \brief Set room interface and set active client for room interface
 */

/*!
 * \fn void FKUserInterface::showMessage(const QString& m)
 * \brief Show message for user
 */

/*!
 * \fn void FKUserInterface::startRealmRequested()
 * \brief Emits when user requested realm creation
 */

/*!
 * \fn void FKUserInterface::startClientInfrastructureRequested()
 * \brief Emits when user requested connection to realm as client
 */

/*!
 * \fn void FKUserInterface::createUserRequested(const QString userName)
 * \brief Emits when new user record creation requested
 */

/*!
 * \fn void FKUserInterface::deleteUserRequested(const QString userName)
 * \brief Emits when existing user record deletion requested
 */

/*!
 * \fn void FKUserInterface::clientLoginRequested(const QString clientName,const QString password)
 * \brief Emits when current client infrastructure realm autentification requested
 */

/*!
 * \fn void FKUserInterface::createClientRequested(const QString clientName,const QString password)
 * \brief Emits when realm administrator requests new client record creation
 */

/*!
 * \fn void FKUserInterface::quitApplicationRequested()
 * \brief Signal emitted when quit application command recieved
 */
