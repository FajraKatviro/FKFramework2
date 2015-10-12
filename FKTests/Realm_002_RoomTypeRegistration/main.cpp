#include <QtTest/QtTest>
#include "FKConsoleTester.h"

class Tester:public FKConsoleTester{
    Q_OBJECT
private slots:
    void test();
};

void Tester::test(){
    sendCommand("start realm");
    sendCommand("add room type roomtype");
    sendCommand("add room type testType");
    sendCommand("add room type testType rt");
    QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().size(), 3);
    QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().contains("roomtype"), true);
    QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().contains("testtype"), true);
    QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().contains("rt"), true);
    sendCommand("remove room type testType");
    QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().size(), 2);
    QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().contains("testtype"), false);
    sendCommand("add room type testType");
    QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().size(), 3);
    QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().contains("testtype"), true);
    sendCommand("remove room type testType");
    sendCommand("remove room type testType");
    QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().size(), 2);
    sendCommand("add room type testTyp.%");
    QTest::qWait(500);
    QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().size(), 2);
    sendCommand("remove room type testTyp.%");
    QTest::qWait(500);
    QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().size(), 2);
    sendCommand("remove room type roOmtype      rt   ");
    QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().size(), 0);
    QTest::qWait(500);
}

QTEST_MAIN(Tester)
#include "main.moc"
