#include "FKConsoleInterface.h"

#include <QTextStream>

#include "FKConsoleReader.h"

#include "FKCommands.h"
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
    _consoleReader=new FKConsoleReader();
    _consoleReader->moveToThread(&_consoleReaderThread);
    connect(&_consoleReaderThread,SIGNAL(finished()),_consoleReader,SLOT(deleteLater()));
    connect(_consoleReader,SIGNAL(gotLine(QString)),SLOT(processInput(QString)));
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

void FKConsoleInterface::processInput(QString input){
    if(isCommand(input,FKCommands::quit)){
        emit quitApplicationRequested();
    }else if(isCommand(input,FKCommands::help)){
        printHelp();
    }else if(isCommand(input,FKCommands::startRealm)){
        emit startRealmRequested();
    }else if(isCommand(input,FKCommands::startClient)){
        emit startClientInfrastructureRequested();
    }else if(isCommand(input,FKCommands::createUser)){
        requestCreateUser(input);
    }else if(isCommand(input,FKCommands::deleteUser)){
        requestDeleteUser(input);
    }else if(isCommand(input,FKCommands::showUsers)){
        showUsers(input);
    }else if(isCommand(input,FKCommands::loginClient)){
        loginClient(input);
    }else if(isCommand(input,FKCommands::createClient)){
        createClient(input);
    }else{
        showMessage(QString(tr("Unknown command")));
    }
}

QString FKConsoleInterface::welcomeString() const{
    return QString(tr("Welcome to FKLudo application!\nType '%1' command to show help message")).arg(FKCommands::help);
}

void FKConsoleInterface::printHelp(){
    const qint32 commandWidth=16;
    showMessage(QString(tr("Use following commands to manage application:\n"))+
                QString(tr("%1\tquit application\n")).arg(FKCommands::quit.rightJustified(commandWidth))+
                QString(tr("%1\tshow this help message\n")).arg(FKCommands::help.rightJustified(commandWidth))+
                QString(tr("%1\tstart realm infrastructure\n")).arg(FKCommands::startRealm.rightJustified(commandWidth))+
                QString(tr("%1\tcreate new client record for started realm\n")).arg(FKCommands::createClient.rightJustified(commandWidth))+
                QString(tr("%1\tstart client infrastructure\n")).arg(FKCommands::startClient.rightJustified(commandWidth))+
                QString(tr("%1\tsubmit current client on connected realm\n")).arg(FKCommands::loginClient.rightJustified(commandWidth))+
                QString(tr("%1\tcreate new user for current client\n")).arg(FKCommands::createUser.rightJustified(commandWidth))+
                QString(tr("%1\tdelete existing user for current client\n")).arg(FKCommands::deleteUser.rightJustified(commandWidth))+
                QString(tr("%1\tshow list of existing users. Use %2 option to show all users registered on realm")).arg(FKCommands::showUsers.rightJustified(commandWidth)).arg(FKCommandOptions::realm)
                );
}

bool FKConsoleInterface::isCommand(QString& arg, const QString& command){
    //if(QRegExp(someExpr).exactMatch(arg)){
    if(arg==command || arg.startsWith(command+QString(" "))){
        arg.remove(0,command.length()+1);
        return true;
    }
    return false;
}

bool FKConsoleInterface::hasKey(QString& arg, const QString& key){
    const QChar space(' ');
    const QString oldValue=arg.prepend(space).append(space);
    if(oldValue!=arg.replace(QRegExp(QString("\\s%1\\s").arg(key)),space)){
        arg=arg.trimmed();
        return true;
    }
    return false;
}

void FKConsoleInterface::showUsers(QString& arg){
    arg=arg.trimmed();
    if(arg.isEmpty()){
        emit clientUsersRequested();
    }else if(hasKey(arg,FKCommandOptions::realm)){
        emit realmUsersRequested();
    }else{
        showMessage(QString(tr("Invalid argument specified: %1")).arg(arg));
    }
}

void FKConsoleInterface::loginClient(const QString& arg){
    QStringList splitArg=arg.trimmed().split(' ',QString::SkipEmptyParts);
    if(splitArg.size()==2){
        emit clientLoginRequested(splitArg.at(0),splitArg.at(1));
    }else{
        showMessage(QString(tr("You need provide 2 arguments: login and password")));
    }
}

void FKConsoleInterface::createClient(const QString& arg){
    QStringList splitArg=arg.trimmed().split(' ',QString::SkipEmptyParts);
    if(splitArg.size()==2){
        emit createClientRequested(splitArg.at(0),splitArg.at(1));
    }else{
        showMessage(QString(tr("You need provide 2 arguments: login and password")));
    }
}
