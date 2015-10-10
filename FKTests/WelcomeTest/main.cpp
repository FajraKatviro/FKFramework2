#include <QtTest/QtTest>
#include "FKConsoleTester.h"

class Tester:public FKConsoleTester{
    Q_OBJECT
private slots:
    void welcomeTest();
};

void Tester::welcomeTest(){
    sendCommand("start realm");
    bool realmStarted=app.getCore()->realmComponent()->isRunning();
    QCOMPARE(realmStarted, true);
}

QTEST_MAIN(Tester)
#include "main.moc"
