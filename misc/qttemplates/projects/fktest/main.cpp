#include <QtTest/QtTest>
#include "FKConsoleTester.h"

class Tester:public FKConsoleTester{
    Q_OBJECT
private slots:
    void test();
};

void Tester::test(){
    sendCommand("start realm");

    QCOMPARE(1, 1);
}

QTEST_MAIN(Tester)
#include "main.moc"
