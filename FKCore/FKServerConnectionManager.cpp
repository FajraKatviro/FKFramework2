#include "FKServerConnectionManager.h"

#include "FKServerInfrastructure.h"
#include "FKClientInfrastructureReferent.h"
#include "FKMessage.h"
#include "FKBasicEvent.h"
#include "FKEventObject.h"

#include "FKLogger.h"
#include "FKBasicEventSubjects.h"

/*!
\class FKServerConnectionManager
\brief This class used to process guest connectors at server-side
*/

/*!
 * \brief Create manager for \i connector at \i server with \i parent
 */

FKServerConnectionManager::FKServerConnectionManager(FKServerInfrastructure *server, FKConnector *connector, QObject *parent):
        FKConnectionManager(connector,parent),_server(server){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Deletes manager
 */

FKServerConnectionManager::~FKServerConnectionManager(){
    FK_DBEGIN
    FK_DEND
}

void FKServerConnectionManager::processMessage(FKMessage* msg){
    FK_MLOGV("Unexpected message from guest to server",msg->subject())
    msg->deleteLater();
}

void FKServerConnectionManager::processGuestEvent(FKBasicEvent* ev){
    const QString subject=ev->subject();
    const QVariant value=ev->value();
    ev->deleteLater();
    if(subject==FKBasicEventSubject::login){
        _server->syncRequest(this,value);
    }else{
        FK_MLOGV("Unexpected guest event subject from guest to server",subject)
        _server->stopGuestConnection(this);
    }
}

void FKServerConnectionManager::processBasicEvent(FKBasicEvent* ev){
    FK_MLOGV("Unexpected basic event from guest to server",ev->subject())
    ev->deleteLater();
}

void FKServerConnectionManager::processEvent(FKEventObject* ev){
    FK_MLOGV("Unexpected event from guest to server",ev->subject())
    ev->deleteLater();
}

void FKServerConnectionManager::incomeMessageError(const QString& msgType, const QString& reason){
    FK_MLOGV(QString("Income message error from guest to server: ")+reason,msgType)
}

/*!
\class FKServerConnectionManagerR
\brief This class used to process realm connector at server-side
*/

/*!
 * \brief Create manager for \i connector at \i server with \i parent
 */

FKServerConnectionManagerR::FKServerConnectionManagerR(FKServerInfrastructure *server, FKConnector *connector, QObject *parent):
        FKConnectionManager(connector,parent),_server(server){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Deletes manager
 */

FKServerConnectionManagerR::~FKServerConnectionManagerR(){
    FK_DBEGIN
    FK_DEND
}

void FKServerConnectionManagerR::processMessage(FKMessage* msg){
    _server->messageFromRealm(msg->subject());
    msg->deleteLater();
}

void FKServerConnectionManagerR::processGuestEvent(FKBasicEvent* ev){
    FK_MLOGV("Unexpected guest event from realm to server",ev->subject())
    ev->deleteLater();
}

void FKServerConnectionManagerR::processBasicEvent(FKBasicEvent* ev){
    const QString subject=ev->subject();
    const QVariant value=ev->value();
    ev->deleteLater();
    if(subject==FKBasicEventSubject::login){
        _server->submitLoginRealm(value);
    }else if(subject==FKBasicEventSubject::registerRoomType){
        _server->registerRoomTypeRespond(value);
    }else if(subject==FKBasicEventSubject::removeRoomType){
        _server->removeRoomTypeRespond(value);
    }else if(subject==FKBasicEventSubject::createRoom){
        _server->createRoomRequested(value);
    }else if(subject==FKBasicEventSubject::joinRoom){
        _server->clientInvited(value);
    }else if(subject==FKBasicEventSubject::roomTypeList){
        _server->roomTypesNotification(value);
    }else{
        FK_MLOGV("Unexpected basic event subject from realm to server",subject)
    }
}

void FKServerConnectionManagerR::processEvent(FKEventObject* ev){
    FK_MLOGV("Unexpected event from realm to server",ev->subject())
    ev->deleteLater();
}

void FKServerConnectionManagerR::incomeMessageError(const QString& msgType, const QString& reason){
    FK_MLOGV(QString("Income message error from realm to server: ")+reason,msgType)
}

/*!
\class FKServerConnectionManagerU
\brief This class used to process connectors from users at server-side
*/

/*!
 * \brief Create manager for \i connector at \i userSlot with \i parent
 */

FKServerConnectionManagerU::FKServerConnectionManagerU(FKClientInfrastructureReferent* referent, FKConnector *connector, QObject *parent):
        FKConnectionManager(connector,parent),_referent(referent){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Deletes manager
 */

FKServerConnectionManagerU::~FKServerConnectionManagerU(){
    FK_DBEGIN
    FK_DEND
}

void FKServerConnectionManagerU::processMessage(FKMessage* msg){
    FK_MLOGV("Unexpected message from user to client referent",msg->subject())
    msg->deleteLater();
}

void FKServerConnectionManagerU::processGuestEvent(FKBasicEvent* ev){
    FK_MLOGV("Unexpected guest event from user to client referent",ev->subject())
    ev->deleteLater();
}

void FKServerConnectionManagerU::processBasicEvent(FKBasicEvent* ev){
    const QString subject=ev->subject();
    const QVariant value=ev->value();
    ev->deleteLater();
    if(false/*subject==FKBasicEventSubject::_____*/){
        todo;// reconnect event, quit event
    }else{
        FK_MLOGV("Unexpected basic event subject from user to client referent",subject)
    }
}

void FKServerConnectionManagerU::processEvent(FKEventObject* ev){
    //_referent->incomeAction(ev);
}

void FKServerConnectionManagerU::incomeMessageError(const QString& msgType, const QString& reason){
    FK_MLOGV(QString("Income message error from user to client referent: ")+reason,msgType)
}
