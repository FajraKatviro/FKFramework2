#include "FKConsoleInterface.h"

#include <QTextStream>

#include "FKConsoleReader.h"
#include "FKCommandResolver.h"

#include "FKLogger.h"

/*!
\class FKConsoleInterface
\brief Default userinterface for console applications. Class provides accesss to common features of FK-applications, such as realm or server driving, creating profiles or content updating and all other stuff, exept of FKMultimedia module based features.
*/

/*!
 * \brief Creates instance of interface. Usually, this object have to be created by QML automaticly.
 */

FKConsoleInterface::FKConsoleInterface(QObject *parent):FKUserInterface(parent){
    FK_CBEGIN
    auto resolver=new FKCommandResolver(this);
    installCommandsResolver(resolver);
    _consoleReader=new FKConsoleReader();
    _consoleReader->moveToThread(&_consoleReaderThread);
    connect(&_consoleReaderThread,SIGNAL(finished()),_consoleReader,SLOT(deleteLater()));
    connect(_consoleReader,SIGNAL(gotLine(QString)),resolver,SLOT(processInput(QString)));
    _consoleReaderThread.start();
    FK_CEND
}

/*!
 * \brief Show final message and deletes object
 */

FKConsoleInterface::~FKConsoleInterface(){
    FK_DBEGIN
    showMessage(QString(tr("Application execution complete. Press \"ENTER\" to exit.")));
    _consoleReader->setEnabled(false);
    _consoleReaderThread.quit();
    _consoleReaderThread.wait();
    FK_DEND
}

/*!
 * \brief Print given message to console
 */

void FKConsoleInterface::showMessage(const QString& m){
    QTextStream out(stdout);
    out<<m<<endl;
}

void FKConsoleInterface::showUpdateData(){
    showMessage("Update data recieved");
}

/*!
 * \fn void FKConsoleInterface::clientUsersRequested()
 * \brief Signal emitted when showUsers command recognized for active client
 */

/*!
 * \fn void FKConsoleInterface::realmUsersRequested()
 * \brief Signal emitted when showUsers command recognized for realm
 */

/*!
 * \brief Print welcome message and start read commands from console.
 */

void FKConsoleInterface::show(){
    showMessage(welcomeString());
    _consoleReader->setEnabled();
}

