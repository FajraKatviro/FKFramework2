#include "FKInfrastructure.h"

#include "FKDataBase.h"

#include "FKLogger.h"

/*!
\class FKInfrastructure
\brief This is basic class for all core component infrastructures. Class provides request-answer approach.
*/

/*!
 * \brief Creates disconnected infrastructure object
 */

FKInfrastructure::FKInfrastructure(QObject *parent):QObject(parent),_db(0){
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

/*!
 * \brief Set database containing infrastructure data
 */

void FKInfrastructure::setDataBase(FKDataBase* db){
    _db=db;
    sanityDatabase();
}

QString FKInfrastructure::createRandomString(const qint32 minLen, const qint32 maxLen){
    static QString letters;
    static qint32 size;
    if(letters.isEmpty()){
        for(char i='a';i<='z';++i){
            letters.append(QChar(i));
        }
        for(char i='A';i<='Z';++i){
            letters.append(QChar(i));
        }
        size=letters.size();
    }

    qint32 len=qrand()%(maxLen-minLen+1)+minLen;
    QString str;
    for(qint32 i=0;i<len;++i){
        str.append(letters.at(qrand()%(size+1)));
    }
    return str;
}

bool FKInfrastructure::isClientId(const QString& client){
    return FKDBIndex::isNodeName(client);
}

bool FKInfrastructure::isUserName(const QString& user){
    return FKDBIndex::isNodeName(user);
}

bool FKInfrastructure::isRoomName(const QString& room){
    return FKDBIndex::isNodeName(room);
}

bool FKInfrastructure::isRoomTypeName(const QString& roomType){
    return FKDBIndex::isNodeName(roomType);
}

/*!
 * \brief Return previously set database
 */

FKDataBase* FKInfrastructure::database()const{
    return _db;
}
