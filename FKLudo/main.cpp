#include <QCoreApplication>

#include "FKApplication.h"
#include "FKSimpleCore.h"
#include "FKConsoleInterface.h"
#include "FKRoomEngine.h"

int main(int argc, char** argv){
    QCoreApplication qapp(argc,argv);

    FKApplication app;
    app.setCore<FKSimpleCore>();
    app.setUserInterface<FKConsoleInterface>();
    app.setRoomEngine<FKRoomEngine>();
    app.setInterfaceBridge(QStringLiteral("ConsoleBridge"));
    app.start();

    return qapp.exec();
}
