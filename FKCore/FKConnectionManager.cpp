#include "FKConnectionManager.h"

#include "FKConnector.h"
#include "FKMessage.h"
#include "FKBasicEvent.h"
#include "FKUpdateData.h"
#include "FKEventObject.h"

#include "FKPacker.h"
#include "FKLogger.h"

/*!
\class FKConnectionManager
\brief Class encapsulates income/outcome message handle for single connection
*/

/*!
 * \brief Creates manager for given connector
 */

FKConnectionManager::FKConnectionManager(FKConnector* connector,QObject *parent):
        QObject(parent),_connector(connector){
    FK_CBEGIN
    connect(connector,SIGNAL(gotMessage(QString,FKSystemObject*)),SLOT(processMsg(QString,FKSystemObject*)));
    connect(connector,SIGNAL(statusChanged(FKConnector::Status)),SLOT(connectionStatusChangedSlot()));
    FK_CEND
}

/*!
 * \brief Deletes manager, not affected connector;
 */

FKConnectionManager::~FKConnectionManager(){
    FK_DBEGIN
    FK_DEND
}

/*!
 * \brief Returns \i true if manager has connected connector
 */

bool FKConnectionManager::isActive() const{
    return _connector && _connector->status()==FKConnector::Connected;
}

/*!
 * \brief Send pre-packed SystemObject
 */

void FKConnectionManager::sendData(const QByteArray& data){
    _connector->sendMessage(data);
}

/*!
 * \brief Send given message object
 */

void FKConnectionManager::sendMessage(FKMessage* message){
    _connector->sendMessage(FKPacker::message,message);
}

/*!
 * \brief This is overloaded function. Creates message object with given \i message and send it
 */

void FKConnectionManager::sendMessage(const QString& message){
    FKMessage msg(message);
    sendMessage(&msg);
}

/*!
 * \brief Send given event object
 */

void FKConnectionManager::sendEvent(FKEventObject* event){
    _connector->sendMessage(FKPacker::event,event);
}

/*!
 * \brief Send given basec event as guest infrastructure
 */

void FKConnectionManager::sendGuestEvent(FKBasicEvent* event){
    _connector->sendMessage(FKPacker::guestEvent,event);
}

/*!
 * \brief Send given basec event as logined infrastructure
 */

void FKConnectionManager::sendBasicEvent(FKBasicEvent* event){
    _connector->sendMessage(FKPacker::basicEvent,event);
}

void FKConnectionManager::sendUpdateData(FKUpdateData* data){
    _connector->sendMessage(FKPacker::updateData,data);
}

/*!
 * \brief Disconnect connector and delete it
 */

void FKConnectionManager::dropConnection(){
    _connector->deleteLater();
    _connector=0;
}

QString FKConnectionManager::address() const{
    return _connector ? _connector->address() : QString();
}

/*!
 * \fn void FKConnectionManager::processMsg(const QString msgType,FKSystemMessage* msg)
 * \brief Process income message from connector
 */

void FKConnectionManager::processMsg(const QString msgType, FKSystemObject* msg){
    if(!msg){
        incomeMessageError(msgType,QString("Null message"));
        return;
    }else if(msgType==FKPacker::message){
        FKMessage* message=qobject_cast<FKMessage*>(msg);
        if(message){
            processMessage(message);
            return;
        }
    }else if(msgType==FKPacker::guestEvent){
        FKBasicEvent* basicEvent=qobject_cast<FKBasicEvent*>(msg);
        if(basicEvent){
            processGuestEvent(basicEvent);
            return;
        }
    }else if(msgType==FKPacker::basicEvent){
        FKBasicEvent* basicEvent=qobject_cast<FKBasicEvent*>(msg);
        if(basicEvent){
            processBasicEvent(basicEvent);
            return;
        }
    }else if(msgType==FKPacker::event){
        FKEventObject* event=qobject_cast<FKEventObject*>(msg);
        if(event){
            processEvent(event);
            return;
        }
    }else if(msgType==FKPacker::updateData){
        FKUpdateData* data=qobject_cast<FKUpdateData*>(msg);
        if(data){
            processUpdateData(data);
            return;
        }
    }else{
        incomeMessageError(msgType,QString("Invalid msgType"));
        msg->deleteLater();
        return;
    }
    incomeMessageError(msgType,QString("Unable cast"));
    msg->deleteLater();
}

/*!
 * \brief Income message handler. Msg never becomes null.
 */

void FKConnectionManager::processMessage(FKMessage* msg){
    FK_MLOGV("Got message",msg->subject())
    msg->deleteLater();
}

/*!
 * \brief Income guest event handler. Ev never becomes null.
 */

void FKConnectionManager::processGuestEvent(FKBasicEvent* ev){
    FK_MLOGV("Unhandled guest event",ev->subject())
    ev->deleteLater();
}

/*!
 * \brief Income basic event handler. Ev never becomes null.
 */

void FKConnectionManager::processBasicEvent(FKBasicEvent* ev){
    FK_MLOGV("Unhandled basic event",ev->subject())
    ev->deleteLater();
}

/*!
 * \brief Income event handler. Ev never becomes null, but can be invalid.
 */

void FKConnectionManager::processEvent(FKEventObject* ev){
    FK_MLOGV("Unhandled event",ev->subject())
    ev->deleteLater();
}

void FKConnectionManager::processUpdateData(FKUpdateData* data){
    FK_MLOGV("Unhandled update data",data->path())
    data->deleteLater();
}

/*!
 * \brief Income error handler. This function called whenever invalid income data recieved.
 */

void FKConnectionManager::incomeMessageError(const QString& msgType, const QString& reason){
    QString msg=QString("Type: %1; reason: %2").arg(msgType).arg(reason);
    FK_MLOGV("Unhandled income message error",msg)
}

/*!
 * \fn FKConnector* FKConnectionManager::connector()const
 * \brief Returns managing connector
 */

/*!
 * \fn void FKConnectionManager::connectionStatusChanged()
 * \brief This signal emitted when connector changes it's connection status
 */

void FKConnectionManager::connectionStatusChangedSlot(){
    emit connectionStatusChanged();
}
