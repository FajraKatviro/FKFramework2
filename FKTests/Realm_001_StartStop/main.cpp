#include <QtTest/QtTest>
#include "FKConsoleTester.h"

class Tester:public FKConsoleTester{
    Q_OBJECT
private slots:
    void test();
};

void Tester::test(){
    sendCommand("start realm");
    QTRY_VERIFY(app.getCore()->realmComponent()->isRunning());

    sendCommand("stop realm");
    QTRY_VERIFY(!app.getCore()->realmComponent()->isRunning());

    sendCommand("start realm");
    QTRY_VERIFY(app.getCore()->realmComponent()->isRunning());

    sendCommand("stop realm");
    QTRY_VERIFY(!app.getCore()->realmComponent()->isRunning());

    sendCommand("start realm");
    sendCommand("stop realm");
    QTRY_VERIFY(!app.getCore()->realmComponent()->isRunning());

    sendCommand("start realm");
    sendCommand("start realm");
    QTRY_VERIFY(app.getCore()->realmComponent()->isRunning());

    sendCommand("stop realm");
    sendCommand("stop realm");
    QTRY_VERIFY(!app.getCore()->realmComponent()->isRunning());
}

QTEST_MAIN(Tester)
#include "main.moc"

