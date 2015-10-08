#include <QtTest/QtTest>

class Tester: public QObject{
    Q_OBJECT
private slots:
    void welcomeTest();
};

void Tester::welcomeTest()
{
    QCOMPARE(1, 1);
}

QTEST_MAIN(Tester)
#include "main.moc"
