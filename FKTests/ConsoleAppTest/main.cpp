#include <QtTest/QtTest>
#include "FKConsoleTester.h"

class Tester:public FKConsoleTester{
    Q_OBJECT
private slots:
    void realmStarted();
    void realmStarted_data();

    void realmRoomTypeRegistered();
    void realmRoomTypeRegistered_data();

    void realmRoomTypesCount();
    void realmRoomTypesCount_data();
private:
};

void Tester::realmStarted_data(){
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("result");

    loadTestData(2);
}
void Tester::realmStarted(){
    QFETCH(QString,command);
    QFETCH(QString,result);

    sendCommand(command);
    QTRY_COMPARE(app.getCore()->realmComponent()->isRunning(),result=="true");
}

void Tester::realmRoomTypeRegistered_data(){
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("arg");
    QTest::addColumn<QString>("containTest");
    QTest::addColumn<QString>("result");

    loadTestData(4);
}
void Tester::realmRoomTypeRegistered(){
    QFETCH(QString,command);
    QFETCH(QString,arg);
    QFETCH(QString,containTest);
    QFETCH(QString,result);

    sendCommand(command+" "+arg);
    QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().contains(containTest), result=="true");
}

void Tester::realmRoomTypesCount_data(){
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("result");

    loadTestData(2);
}
void Tester::realmRoomTypesCount(){
    QFETCH(QString,command);
    QFETCH(QString,result);

    sendCommand(command);
    QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().size(),result.toInt());
}

QTEST_MAIN(Tester)
#include "main.moc"

