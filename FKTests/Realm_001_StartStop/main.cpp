#include <QtTest/QtTest>
#include "FKConsoleTester.h"

class Tester:public FKConsoleTester{
    Q_OBJECT
private slots:
    void test();
};

void Tester::test(){
    sendCommand("start realm");
    QVERIFY2(app.getCore()->realmComponent()->isRunning(),
             "Start failed");

    sendCommand("stop realm");
    QVERIFY2(!app.getCore()->realmComponent()->isRunning(),
             "Stop failed");

    sendCommand("start realm");
    QVERIFY2(app.getCore()->realmComponent()->isRunning(),
             "Restart failed");

    sendCommand("stop realm");
    QVERIFY2(!app.getCore()->realmComponent()->isRunning(),
             "Restop failed");

    sendCommand("start realm");
    sendCommand("stop realm");
    QVERIFY2(!app.getCore()->realmComponent()->isRunning(),
             "Fast calls failed");

    sendCommand("start realm");
    sendCommand("start realm");
    QVERIFY2(app.getCore()->realmComponent()->isRunning(),
             "Double start failed");

    sendCommand("stop realm");
    sendCommand("stop realm");
    QVERIFY2(!app.getCore()->realmComponent()->isRunning(),
             "Double stop failed");
}

QTEST_MAIN(Tester)
#include "main.moc"

