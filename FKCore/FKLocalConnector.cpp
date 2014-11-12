#include "FKLocalConnector.h"

#include "FKSystemObject.h"
#include "FKPacker.h"

#include "FKLogger.h"

/*!
 * \brief Creates connector instance
 */

FKLocalConnector::FKLocalConnector(QObject *parent):FKConnector(parent){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Deletes connector object
 */

FKLocalConnector::~FKLocalConnector(){
    FK_DBEGIN
    FK_DEND
}

/*!
 * \brief This is reimplemented function. Send message to other connected local connector
 */

void FKLocalConnector::sendMessage(const QString& msgType, FKSystemObject* msg){
    emit transmitMessage(msgType,msg);
}

/*!
 * \brief This is reimplemented function. For local connector to improve performance use sendMessage(QString,FKSystemObject*) function instead of this one, it let you avoid expensive pack and unpack operations
 */

void FKLocalConnector::sendMessage(QByteArray data){
    emit transmitMessage(data);
}

/*!
 * \brief Create connection to another disconnected local connector
 */

void FKLocalConnector::join(FKLocalConnector* other){
    if(status()!=Disconnected || other->status()!=Disconnected){
        FK_MLOG("Unable join already connected connector")
        return;
    }
    connect(this,SIGNAL(transmitMessage(QString,FKSystemObject*)),other,SLOT(readMessage(QString,FKSystemObject*)),Qt::DirectConnection);
    connect(this,SIGNAL(transmitMessage(QByteArray)),other,SLOT(readMessage(QByteArray)),Qt::DirectConnection);
    connect(this,SIGNAL(statusChanged(FKConnector::Status)),other,SLOT(setStatus(FKConnector::Status)));
    connect(other,SIGNAL(transmitMessage(QString,FKSystemObject*)),this,SLOT(readMessage(QString,FKSystemObject*)),Qt::DirectConnection);
    connect(other,SIGNAL(transmitMessage(QByteArray)),this,SLOT(readMessage(QByteArray)),Qt::DirectConnection);
    connect(other,SIGNAL(statusChanged(FKConnector::Status)),this,SLOT(setStatus(FKConnector::Status)));
    setStatus(Connected);
}

/*!
 * \fn void FKLocalConnector::transmitMessage(const QByteArray& data)
 * \brief This signal transmits data to another local connector and shouldn't be used in code
 */

/*!
 * \fn void FKLocalConnector::transmitMessage(const QString& msgType, FKSystemObject* msg)
 * \brief This signal transmits data to another local connector and shouldn't be used in code
 */

void FKLocalConnector::readMessage(const QByteArray& data){
    FKSystemObject* msg=0;
    const QString msgType=FKPacker::unpack(data,msg);
    emit gotMessage(msgType,msg);
}

void FKLocalConnector::readMessage(const QString& msgType, FKSystemObject* msg){
    FKSystemObject* obj= msg ? msg->clone() : 0;
    emit gotMessage(msgType,obj);
}
