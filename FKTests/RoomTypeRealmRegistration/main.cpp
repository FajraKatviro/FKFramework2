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
    QStringList roomTypes=app.getCore()->realmComponent()->roomTypeList();
    QCOMPARE(roomTypes.size(), 3);
    QCOMPARE(roomTypes.contains("roomtype"), true);
    QCOMPARE(roomTypes.contains("testtype"), true);
    QCOMPARE(roomTypes.contains("rt"), true);
}

QTEST_MAIN(Tester)
#include "main.moc"
