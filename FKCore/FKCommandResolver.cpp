#include "FKCommandResolver.h"

#include "FKCommands.h"

FKCommandResolver::FKCommandResolver(QObject *parent):QObject(parent){}


void FKCommandResolver::processInput(QString input){
    if(isCommand(input,FKCommands::quit)){
        emit quitApplicationRequested();
    }else if(isCommand(input,FKCommands::help)){
        printHelp();
    }else if(isCommand(input,FKCommands::startRealm)){
        emit startRealmRequested();
    }else if(isCommand(input,FKCommands::startClient)){
        emit startClientInfrastructureRequested();
    }else if(isCommand(input,FKCommands::createUser)){
        //requestCreateUser(input);
    }else if(isCommand(input,FKCommands::deleteUser)){
        //requestDeleteUser(input);
    }else if(isCommand(input,FKCommands::showUsers)){
        showUsers(input);
    }else if(isCommand(input,FKCommands::loginClient)){
        loginClient(input);
    }else if(isCommand(input,FKCommands::createClient)){
        createClient(input);
    }else if(isCommand(input,FKCommands::deleteClient)){
        deleteClient(input);
    }else if(isCommand(input,FKCommands::createServer)){
        createServer(input);
    }else if(isCommand(input,FKCommands::deleteServer)){
        deleteServer(input);
    }else if(isCommand(input,FKCommands::startServer)){
        emit startServerInfrastructureRequested();
    }else if(isCommand(input,FKCommands::loginServer)){
        loginServer(input);
    }else if(isCommand(input,FKCommands::createRoom)){
        createRoom(input);
    }else if(isCommand(input,FKCommands::joinRoom)){
        joinRoom(input);
    }else if(isCommand(input,FKCommands::registerRoomType)){
        registerRoomType(input);
    }else if(isCommand(input,FKCommands::removeRoomType)){
        removeRoomType(input);
    }else if(isCommand(input,FKCommands::stopRealm)){
        emit stopRealmRequested();
    }else{
        emit message(QString(tr("Unknown command")));
    }
}

QString FKCommandResolver::welcomeString() const{
    return QString(tr("Welcome to FKLudo application!\nType '%1' command to show help message")).arg(FKCommands::help);
}

void FKCommandResolver::printHelp(){
    const qint32 commandWidth=16;
    emit message(QString(tr("Use following commands to manage application:\n"))+
                QString(tr("%1\tquit application\n")).arg(FKCommands::quit.rightJustified(commandWidth))+
                QString(tr("%1\tshow this help message\n")).arg(FKCommands::help.rightJustified(commandWidth))+
                QString(tr("%1\tstart realm infrastructure\n")).arg(FKCommands::startRealm.rightJustified(commandWidth))+
                QString(tr("%1\tstop realm infrastructure\n")).arg(FKCommands::stopRealm.rightJustified(commandWidth))+
                QString(tr("%1\tcreate new client record for started realm\n")).arg(FKCommands::createClient.rightJustified(commandWidth))+
                QString(tr("%1\tdelete client record for started realm\n")).arg(FKCommands::deleteClient.rightJustified(commandWidth))+
                QString(tr("%1\tcreate new server record for started realm\n")).arg(FKCommands::createServer.rightJustified(commandWidth))+
                QString(tr("%1\tdelete server record for started realm\n")).arg(FKCommands::deleteServer.rightJustified(commandWidth))+
                QString(tr("%1\tregister room type for started realm. Use %2 option to register avaliable room type for current server\n")).arg(FKCommands::registerRoomType.rightJustified(commandWidth)).arg(FKCommandOptions::server)+
                QString(tr("%1\tremove room type for started realm. Use %2 option to remove registered room type from current server\n")).arg(FKCommands::removeRoomType.rightJustified(commandWidth)).arg(FKCommandOptions::server)+
                QString(tr("%1\tstart client infrastructure\n")).arg(FKCommands::startClient.rightJustified(commandWidth))+
                QString(tr("%1\tstart server infrastructure\n")).arg(FKCommands::startServer.rightJustified(commandWidth))+
                QString(tr("%1\tsubmit current client on connected realm\n")).arg(FKCommands::loginClient.rightJustified(commandWidth))+
                QString(tr("%1\tsubmit current server on connected realm\n")).arg(FKCommands::loginServer.rightJustified(commandWidth))+
                QString(tr("%1\tcreate new user for current client\n")).arg(FKCommands::createUser.rightJustified(commandWidth))+
                QString(tr("%1\tdelete existing user for current client\n")).arg(FKCommands::deleteUser.rightJustified(commandWidth))+
                QString(tr("%1\tjoin existing room by current client\n")).arg(FKCommands::joinRoom.rightJustified(commandWidth))+
                QString(tr("%1\tcreate room for current client. Realm admin should use %2 option to create empty room for random server\n")).arg(FKCommands::createRoom.rightJustified(commandWidth)).arg(FKCommandOptions::realm)+
                QString(tr("%1\tshow list of existing users. Use %2 option to show all users registered on realm")).arg(FKCommands::showUsers.rightJustified(commandWidth)).arg(FKCommandOptions::realm)
                );
}

bool FKCommandResolver::isCommand(QString& arg, const QString& command){
    //if(QRegExp(someExpr).exactMatch(arg)){
    if(arg==command || arg.startsWith(command+QString(" "))){
        arg.remove(0,command.length()+1);
        return true;
    }
    return false;
}

bool FKCommandResolver::hasKey(QString& arg, const QString& key){
    const QChar space(' ');
    const QString oldValue=arg.prepend(space).append(space);
    if(oldValue!=arg.replace(QRegExp(QString("\\s%1\\s").arg(key)),space)){
        arg=arg.trimmed();
        return true;
    }
    return false;
}

void FKCommandResolver::showUsers(QString& arg){
    arg=arg.trimmed();
    if(arg.isEmpty()){
        emit clientUsersRequested();
    }else if(hasKey(arg,FKCommandOptions::realm)){
        emit realmUsersRequested();
    }else{
        emit message(QString(tr("Invalid argument specified: %1")).arg(arg));
    }
}

void FKCommandResolver::loginClient(const QString& arg){
    QStringList splitArg=arg.trimmed().split(' ',QString::SkipEmptyParts);
    if(splitArg.size()==2){
        emit clientLoginRequested(splitArg.at(0),splitArg.at(1));
    }else{
        emit message(QString(tr("You need provide 2 arguments: login and password")));
    }
}

void FKCommandResolver::loginServer(const QString& arg){
    QStringList splitArg=arg.trimmed().split(' ',QString::SkipEmptyParts);
    if(splitArg.size()==2){
        emit serverLoginRequested(splitArg.at(0).toInt(),splitArg.at(1));
    }else{
        emit message(QString(tr("You need provide 2 arguments: id and password")));
    }
}

void FKCommandResolver::createClient(const QString& arg){
    QStringList splitArg=arg.trimmed().split(' ',QString::SkipEmptyParts);
    if(splitArg.size()==2){
        emit createClientRequested(splitArg.at(0),splitArg.at(1));
    }else{
        emit message(QString(tr("You need provide 2 arguments: login and password")));
    }
}

void FKCommandResolver::deleteClient(const QString& arg){
    QString clientName=arg.trimmed();
    if(!clientName.isEmpty()){
        emit deleteClientRequested(clientName);
    }else{
        emit message(QString(tr("Unable request client deletion: invalid client name")));
    }
}

void FKCommandResolver::createServer(QString arg){
    arg=arg.trimmed();
    QStringList splitArg=arg.trimmed().split(' ',QString::SkipEmptyParts);
    if(splitArg.size()==1){
        emit createServerRequested(splitArg.first());
    }else{
        emit message(QString(tr("You need provide 1 argument: password")));
    }
}

void FKCommandResolver::deleteServer(QString arg){
    qint32 serverId=arg.trimmed().toInt();
    if(serverId>0){
        emit deleteServerRequested(serverId);
    }else{
        emit message(QString(tr("Unable request server deletion: invalid server id")));
    }
}

void FKCommandResolver::createRoom(QString arg){
    QString argTrimmed=arg.trimmed();
    bool realm=hasKey(argTrimmed,FKCommandOptions::realm);
    QStringList splitArg=argTrimmed.split(' ',QString::SkipEmptyParts);
    if(realm){
        if(splitArg.size()==2){
            emit createRoomRequestedRealm(splitArg.at(0),splitArg.at(1));
        }else{
            emit message(QString(tr("You need provide 2 arguments: room name and room type")));
        }
    }else{
        if(splitArg.size()>2){
            emit createRoomRequested(splitArg.takeFirst(),splitArg.takeFirst(),splitArg);
        }else{
            emit message(QString(tr("You need provide 3 or more arguments: room name, room type and list of users")));
        }
    }
}

void FKCommandResolver::joinRoom(QString arg){
    QString argTrimmed=arg.trimmed();
    QStringList splitArg=argTrimmed.split(' ',QString::SkipEmptyParts);
    if(splitArg.size()>1){
        emit joinRoomRequested(splitArg.takeFirst(),splitArg);
    }else{
        emit message(QString(tr("You need provide 2 or more arguments: room name and list of users")));
    }
}

void FKCommandResolver::registerRoomType(QString arg){
    arg=arg.trimmed();
    bool server=hasKey(arg,FKCommandOptions::server);
    QStringList splitArg=arg.trimmed().split(' ',QString::SkipEmptyParts);
    if(splitArg.isEmpty()){
        emit message(QString(tr("You need provide room type(s)")));
    }else{
        foreach(QString rt,splitArg){
            if(server){
                emit registerServerRoomTypeRequested(rt);
            }else{
                emit registerRoomTypeRequested(rt);
            }
        }
    }
}

void FKCommandResolver::removeRoomType(QString arg){
    arg=arg.trimmed();
    bool server=hasKey(arg,FKCommandOptions::server);
    QStringList splitArg=arg.trimmed().split(' ',QString::SkipEmptyParts);
    if(splitArg.isEmpty()){
        emit message(QString(tr("You need provide room type(s)")));
    }else{
        foreach(QString rt,splitArg){
            if(server){
                emit removeServerRoomTypeRequested(rt);
            }else{
                emit removeRoomTypeRequested(rt);
            }
        }
    }
}
