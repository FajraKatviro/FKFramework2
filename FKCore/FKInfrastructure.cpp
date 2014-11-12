#include "FKInfrastructure.h"

#include "FKLogger.h"

/*!
\class FKInfrastructure
\brief This is basic class for all core component infrastructures. Class provides request-answer approach.
*/

/*!
 * \brief Creates disconnected infrastructure object
 */

FKInfrastructure::FKInfrastructure(QObject *parent):QObject(parent){
    FK_DBEGIN
    FK_CEND
}

/*!
 * \brief Deletes infrastructure object
 */

FKInfrastructure::~FKInfrastructure(){
    FK_DBEGIN
    FK_DEND
}

/*!
\enum FKInfrastructureType

\brief Determines type of infrastructure

\value Realm
        Realm infrastructure
\value Server
        Server infrastructure
\value Client
        Client infrastructure
\value User
        This value represents application active client instances
*/

/*!
 * \fn FKInfrastructureType FKInfrastructure::infrastructureType()const;
 * \brief This function must be reimplemented to return apropriate infrastructure type
 */

/*!
 * \brief Returns true if core is waiting for realm's answer
 */

bool FKInfrastructure::waitingForAnswer(FKInfrastructureType t)const{
    return !_requests.value(t).isEmpty();
}

/*!
 * \brief Set current request to \i subject. Returns \i true on success
 */

bool FKInfrastructure::requestAnswer(FKInfrastructureType t,const QString& subject){
    if(!waitingForAnswer(t)){
        _requests[t]=subject;
        emit waitingForAnswerChanged(t);
        return true;
    }else{
        FK_MLOGV("Unable request answer: another request in progress",_requests.value(t))
        return false;
    }
}

/*!
 * \brief Close current request if it is equal \i subject. Returns \i true on success
 */

bool FKInfrastructure::submitAnswer(FKInfrastructureType t, const QString& subject){
    if(_requests.value(t)==subject){
        cancelAnswer(t);
        return true;
    }else{
        FK_MLOGV("Unable submit answer: unexpected subject "+subject,_requests.value(t))
        return false;
    }
}

/*!
 * \brief Reset current request
 */

void FKInfrastructure::cancelAnswer(FKInfrastructureType t){
    if(!waitingForAnswer(t)){
        FK_MLOG("Unable cancel answer: no request done")
    }else{
        _requests[t].clear();
        emit waitingForAnswerChanged(t);
    }
}

/*!
 * \fn void FKInfrastructure::messageRequested(const QString msg)
 * \brief This signal emitted when infrastructure want show message to user
 */


/*!
 * \fn void FKInfrastructure::waitingForAnswerChanged(FKInfrastructureType infr)
 * \brief This signal emitted when infrastructure creates request or get an answer
 */
