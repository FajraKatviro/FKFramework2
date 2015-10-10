#include "FKApplication.h"

#include "FKMessage.h"
#include "FKBasicEvent.h"
#include "FKEventObject.h"
#include "FKUpdateData.h"
#include "FKObject.h"
#include "FKRoom.h"

#include "FKSimpleCore.h"
#include "FKUserInterface.h"

#include "FKLogger.h"

/*!
\class FKApplication
\brief This is the root component of application that connect core and interface
*/

/*!
 * \brief Constructs empty application object
 */

FKApplication::FKApplication():QObject(),_interface(false),_core(false),_qmlEngine(0){
    FK_CBEGIN
    FK_CEND
}

/*!
 * \brief Deletes FKApplication object
 */

FKApplication::~FKApplication(){
    FK_DBEGIN
    FK_DEND
}

/*!
 * \brief Run initialization and start application. Returns false if core, interface or bridge not set.
 */

bool FKApplication::start(){
    if(_core && _interface && _interfaceBridge!=QString()){
        prepare();
        initLayer();
        return true;
    }
    return false;
}

/*!
 * \brief Set given QML element from recources' /application subfolder as interface bridge.
 */

bool FKApplication::setInterfaceBridge(const QString& bridgeType){
    if(_interfaceBridge==QString()){
        _interfaceBridge=QString("qrc:///application/%1.qml").arg(bridgeType);
        return true;
    }
    return false;
}

FKSimpleCore* FKApplication::getCore() const{
    return _qmlEngine->rootObjects().at(0)->property("core").value<FKSimpleCore*>();
}

FKUserInterface* FKApplication::getUi() const{
    return _qmlEngine->rootObjects().at(0)->property("ui").value<FKUserInterface*>();
}

/*!
 * \brief Make additional initialization
 */

void FKApplication::prepare(){
//install event-objects factory
    FKSystemObject::allowConstructing<FKMessage>();
    FKSystemObject::allowConstructing<FKBasicEvent>();
    FKSystemObject::allowConstructing<FKEventObject>();
    FKSystemObject::allowConstructing<FKUpdateData>();
//install common FKObjects factory
    FKObject::allowService<FKObject>();
    FKObject::allowService<FKRoom>();
//    FKObject::allowCreation<FKUser>();
//    FKObject::allowCreation<FKActor>();
//    FKObject::allowCreation<FKWatchingGroup>();
}

/*!
 * \brief Init QML context and create bridge object (default is singleton UI)
 */

void FKApplication::initLayer(){
    _qmlEngine=new QQmlApplicationEngine(QUrl(_interfaceBridge),this);
}

/*!
 * \fn bool FKApplication::setUserInterface()
 * \brief This template function set given class as UserInterface type for QML-side
 */

/*!
 * \fn bool FKApplication::setCore()
 * \brief This template function set given class as Core type for QML-side
 */
