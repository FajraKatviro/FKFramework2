#include <QtTest/QtTest>

#include "FKApplication.h"
#include "FKSimpleCore.h"
#include "FKConsoleInterface.h"

class Tester: public QObject{
    Q_OBJECT
private slots:
    void welcomeTest();
};

void Tester::welcomeTest(){
    FKApplication app;
    app.setCore<FKSimpleCore>();
    app.setUserInterface<FKConsoleInterface>();
    app.setInterfaceBridge(QStringLiteral("ConsoleBridge"));
    app.start();

    QCOMPARE(1, 1);
}

QTEST_MAIN(Tester)
#include "main.moc"
