#include <QCoreApplication>

#include "FKApplication.h"
#include "FKSimpleCore.h"
#include "FKConsoleInterface.h"

int main(int argc, char** argv){
    QCoreApplication qapp(argc,argv);

    FKApplication app;
    app.setCore<FKSimpleCore>();
    app.setUserInterface<FKConsoleInterface>();
    app.setInterfaceBridge(QStringLiteral("ConsoleBridge"));
    app.start();

    return qapp.exec();
}
