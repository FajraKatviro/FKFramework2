#include "FKUserInterface.h"

#include "FKCommandResolver.h"

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

void FKUserInterface::installCommandsResolver(FKCommandResolver* resolver){
    connect(resolver,&FKCommandResolver::message,this,&FKUserInterface::showMessage);
    connect(resolver,&FKCommandResolver::quitApplicationRequested,this,&FKUserInterface::quitApplicationRequested);
    connect(resolver,&FKCommandResolver::startRealmRequested,this,&FKUserInterface::startRealmRequested);
    connect(resolver,&FKCommandResolver::stopRealmRequested,this,&FKUserInterface::stopRealmRequested);
    connect(resolver,&FKCommandResolver::startClientInfrastructureRequested,this,&FKUserInterface::startClientInfrastructureRequested);
    connect(resolver,&FKCommandResolver::startServerInfrastructureRequested,this,&FKUserInterface::startServerInfrastructureRequested);
    connect(resolver,&FKCommandResolver::clientUsersRequested,this,&FKUserInterface::clientUsersRequested);
    connect(resolver,&FKCommandResolver::realmUsersRequested,this,&FKUserInterface::realmUsersRequested);
    connect(resolver,&FKCommandResolver::clientLoginRequested,this,&FKUserInterface::clientLoginRequested);
    connect(resolver,&FKCommandResolver::serverLoginRequested,this,&FKUserInterface::serverLoginRequested);
    connect(resolver,&FKCommandResolver::createClientRequested,this,&FKUserInterface::createClientRequested);
    connect(resolver,&FKCommandResolver::deleteClientRequested,this,&FKUserInterface::deleteClientRequested);
    connect(resolver,&FKCommandResolver::createServerRequested,this,&FKUserInterface::createServerRequested);
    connect(resolver,&FKCommandResolver::deleteServerRequested,this,&FKUserInterface::deleteServerRequested);
    connect(resolver,&FKCommandResolver::createRoomRequestedRealm,this,&FKUserInterface::createRoomRequestedRealm);
    connect(resolver,&FKCommandResolver::createRoomRequested,this,&FKUserInterface::createRoomRequested);
    connect(resolver,&FKCommandResolver::joinRoomRequested,this,&FKUserInterface::joinRoomRequested);
    connect(resolver,&FKCommandResolver::registerServerRoomTypeRequested,this,&FKUserInterface::registerServerRoomTypeRequested);
    connect(resolver,&FKCommandResolver::registerRoomTypeRequested,this,&FKUserInterface::registerRoomTypeRequested);
    connect(resolver,&FKCommandResolver::removeServerRoomTypeRequested,this,&FKUserInterface::removeServerRoomTypeRequested);
    connect(resolver,&FKCommandResolver::removeRoomTypeRequested,this,&FKUserInterface::removeRoomTypeRequested);
    connect(resolver,&FKCommandResolver::showServerRoomTypesRequested,this,&FKUserInterface::showServerRoomTypesRequested);
    connect(resolver,&FKCommandResolver::showRoomTypesRequested,this,&FKUserInterface::showRoomTypesRequested);
    connect(resolver,&FKCommandResolver::showServersRequested,this,&FKUserInterface::showServersRequested);
    connect(resolver,&FKCommandResolver::showClientsRequested,this,&FKUserInterface::showClientsRequested);
    _commandResolver=resolver;
}

FKCommandResolver*FKUserInterface::commandResolver() const{
    return _commandResolver;
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

void FKUserInterface::requestDeleteUser(const QString& arg){
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
 * \fn void FKUserInterface::startServerInfrastructureRequested()
 * \brief Emits when user requested connection to realm as server
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
 * \fn void FKUserInterface::serverLoginRequested(const qint32 clientName,const QString password)
 * \brief Emits when current server infrastructure realm autentification requested
 */

/*!
 * \fn void FKUserInterface::createClientRequested(const QString clientName,const QString password)
 * \brief Emits when realm administrator requests new client record creation
 */

/*!
 * \fn void FKUserInterface::createRoomRequested(const QString roomName,const QString roomType)
 * \brief Emits when client requests self-side room creation
 */

/*!
 * \fn void FKUserInterface::createServerRequested(const QString password)
 * \brief Emits when realm administrator requests new server record creation
 */

/*!
 * \fn void FKUserInterface::createCustomServerRequested()
 * \brief Emits when client requests custom server starting
 */

/*!
 * \fn void FKUserInterface::quitApplicationRequested()
 * \brief Signal emitted when quit application command recieved
 */

/*!
 * \fn void FKUserInterface::registerRoomTypeRequested(const QString roomType)
 * \brief Signal emitted when realm administrator requests room type regisration
 */

/*!
 * \fn void FKUserInterface::registerServerRoomTypeRequested(const QString roomType)
 * \brief Signal emitted when server administrator requests room type regisration
 */
