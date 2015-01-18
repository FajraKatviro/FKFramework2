#include "FKRealmConnectionManager.h"

#include "FKRealm.h"
#include "FKMessage.h"
#include "FKBasicEvent.h"
#include "FKEvent.h"

#include "FKLogger.h"
#include "FKBasicEventSubjects.h"

/*!
\class FKRealmConnectionManager
\brief This class used to process guest connectors at realm-side
*/

/*!
 * \brief Create manager for \i connector at \i realm with \i parent
 */

FKRealmConnectionManager::FKRealmConnectionManager(FKRealm* realm,FKConnector* connector,QObject *parent):
        FKConnectionManager(connector,parent),_realm(realm){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Deletes manager
 */

FKRealmConnectionManager::~FKRealmConnectionManager(){
    FK_DBEGIN
    FK_DEND
}

void FKRealmConnectionManager::processMessage(FKMessage* msg){
    FK_MLOGV("Unexpected message from guest to realm",msg->subject())
    msg->deleteLater();
    realm()->stopGuestConnection(this);
}

void FKRealmConnectionManager::processGuestEvent(FKBasicEvent* ev){
    const QString subject=ev->subject();
    const QVariant value=ev->value();
    ev->deleteLater();
    if(subject==FKBasicEventSubject::login){
        realm()->ausvise(this,value);
    }else{
        FK_MLOGV("Unexpected guest event subject from guest to realm",subject)
        realm()->stopGuestConnection(this);
    }
}

void FKRealmConnectionManager::processBasicEvent(FKBasicEvent* ev){
    FK_MLOGV("Unexpected basic event from guest to realm",ev->subject())
    ev->deleteLater();
    realm()->stopGuestConnection(this);
}

void FKRealmConnectionManager::processEvent(FKEvent* ev){
    FK_MLOGV("Unexpected event from guest to realm",ev->subject())
    ev->deleteLater();
    realm()->stopGuestConnection(this);
}

void FKRealmConnectionManager::incomeMessageError(const QString& msgType, const QString& reason){
    FK_MLOGV(QString("Income message error from guest to realm: ")+reason,msgType)
    realm()->stopGuestConnection(this);
}

/*!
 * \fn FKRealm* FKRealmConnectionManager::realm()const
 * \brief Returns realm object reference
 */

/*!
\class FKRealmConnectionManagerC
\brief This class used to process connectors from clients at realm-side
*/

/*!
 * \brief Create manager for \i connector at \i realm with \i parent
 */

FKRealmConnectionManagerC::FKRealmConnectionManagerC(const QString& id,FKRealm* realm, FKConnector* connector, QObject* parent):
        FKRealmConnectionManager(realm,connector,parent),_id(id){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Deletes manager
 */

FKRealmConnectionManagerC::~FKRealmConnectionManagerC(){
    FK_DBEGIN
    FK_DEND
}

void FKRealmConnectionManagerC::processMessage(FKMessage* msg){
    FK_MLOGV("Unexpected message from client to realm",msg->subject())
    msg->deleteLater();
    realm()->stopClientConnection(_id);
}

void FKRealmConnectionManagerC::processGuestEvent(FKBasicEvent* ev){
    FK_MLOGV("Unexpected guest event from client to realm",ev->subject())
    ev->deleteLater();
    realm()->stopClientConnection(_id);
}

void FKRealmConnectionManagerC::processBasicEvent(FKBasicEvent* ev){
    const QString subject=ev->subject();
    const QVariant value=ev->value();
    ev->deleteLater();
    if(subject==FKBasicEventSubject::roomList){
        realm()->refreshRoomList(_id,value);
    }else if(subject==FKBasicEventSubject::createUser){
        realm()->createUser(_id,value);
    }else if(subject==FKBasicEventSubject::deleteUser){
        realm()->deleteUser(_id,value);
    }else if(subject==FKBasicEventSubject::selectUser){
        realm()->selectUser(_id,value);
    }else if(subject==FKBasicEventSubject::deselectUser){
        realm()->deselectUser(_id,value);
    }else if(subject==FKBasicEventSubject::customServer){
        realm()->customServerRequested(_id,value);
    }else if(subject==FKBasicEventSubject::createRoom){
        realm()->createRoomRequested(_id,value);
    }else{
        FK_MLOGV("Unexpected basic event subject from client to realm",subject)
        realm()->stopClientConnection(_id);
    }
}

void FKRealmConnectionManagerC::processEvent(FKEvent* ev){
    FK_MLOGV("Unexpected event from client to realm",ev->subject())
    ev->deleteLater();
    realm()->stopClientConnection(_id);
}

void FKRealmConnectionManagerC::incomeMessageError(const QString& msgType, const QString& reason){
    FK_MLOGV(QString("Income message error from client to realm: ")+reason,msgType)
    realm()->stopClientConnection(_id);
}


/*!
\class FKRealmConnectionManagerS
\brief This class used to process connectors from servers at realm-side
*/

/*!
 * \brief Create manager for \i connector at \i realm with \i parent
 */

FKRealmConnectionManagerS::FKRealmConnectionManagerS(const qint32 id,FKRealm* realm, FKConnector* connector, QObject* parent):
        FKRealmConnectionManager(realm,connector,parent),_id(id){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Deletes manager
 */

FKRealmConnectionManagerS::~FKRealmConnectionManagerS(){
    FK_DBEGIN
    FK_DEND
}

void FKRealmConnectionManagerS::processMessage(FKMessage* msg){
    FK_MLOGV("Unexpected message from server to realm",msg->subject())
    msg->deleteLater();
    realm()->stopServerConnection(_id);
}

void FKRealmConnectionManagerS::processGuestEvent(FKBasicEvent* ev){
    FK_MLOGV("Unexpected guest event from server to realm",ev->subject())
    ev->deleteLater();
    realm()->stopServerConnection(_id);
}

void FKRealmConnectionManagerS::processBasicEvent(FKBasicEvent* ev){
    const QString subject=ev->subject();
    const QVariant value=ev->value();
    ev->deleteLater();
    if(subject==FKBasicEventSubject::roomData){
        realm()->refreshRoomData(_id,value);
    }else if(subject==FKBasicEventSubject::createRoom){
        realm()->createRoomRespond(_id,value);
    }else if(subject==FKBasicEventSubject::registerRoomType){
        realm()->registerServerRoomType(_id,value);
    }else{
        FK_MLOGV("Unexpected basic event subject from server to realm",subject)
        realm()->stopServerConnection(_id);
    }
}

void FKRealmConnectionManagerS::processEvent(FKEvent* ev){
    FK_MLOGV("Unexpected event from server to realm",ev->subject())
    ev->deleteLater();
    realm()->stopServerConnection(_id);
}

void FKRealmConnectionManagerS::incomeMessageError(const QString& msgType, const QString& reason){
    FK_MLOGV(QString("Income message error from server to realm: ")+reason,msgType)
    realm()->stopServerConnection(_id);
}
