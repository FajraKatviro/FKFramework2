#ifndef FKCONSOLETESTER
#define FKCONSOLETESTER

#include "FKApplication.h"
#include "FKSimpleCore.h"
#include "FKConsoleInterface.h"
#include "FKCommandResolver.h"

class FKConsoleTester: public QObject{
    Q_OBJECT
public:
    FKConsoleTester(QObject* parent=0):QObject(parent){
        app.setCore<FKSimpleCore>();
        app.setUserInterface<FKConsoleInterface>();
        app.setInterfaceBridge(QStringLiteral("ConsoleBridge"));
        app.start();
        static_cast<FKConsoleInterface*>(app.getUi())->disableConsoleReader();
    }
protected:
    void sendCommand(QString msg){
        app.getUi()->commandResolver()->processInput(msg);
    }
    FKApplication app;
};

#endif // FKCONSOLETESTER

