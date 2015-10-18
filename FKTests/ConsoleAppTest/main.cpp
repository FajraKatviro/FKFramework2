#include <QtTest/QtTest>
#include "FKConsoleTester.h"

class Tester:public FKConsoleTester{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanup();

    void realmStarted();
    void realmStarted_data();
    void realmRoomTypeRegistered();
    void realmRoomTypeRegistered_data();
    void realmServerRegistered();
    void realmServerRegistered_data();
    void realmClientRegistered();
    void realmClientRegistered_data();

    void serverStarted();
    void serverStarted_data();
private:
};

void Tester::initTestCase(){
    QVERIFY(resetDatabase());
}

void Tester::cleanup(){

}

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
    QTest::addColumn<QString>("count");

    loadTestData(5);
}
void Tester::realmRoomTypeRegistered(){
    QFETCH(QString,command);
    QFETCH(QString,arg);
    QFETCH(QString,containTest);
    QFETCH(QString,result);
    QFETCH(QString,count);

    sendCommand(command,arg);
    if(!containTest.isEmpty()){
        QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().contains(containTest), result=="true");
    }
    if(!count.isEmpty()){
        QTRY_COMPARE(app.getCore()->realmComponent()->roomTypeList().size(),count.toInt());
    }
}

void Tester::realmServerRegistered_data(){
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("arg");
    QTest::addColumn<QString>("containTest");
    QTest::addColumn<QString>("result");
    QTest::addColumn<QString>("count");

    loadTestData(5);
}
void Tester::realmServerRegistered(){
    QFETCH(QString,command);
    QFETCH(QString,arg);
    QFETCH(QString,containTest);
    QFETCH(QString,result);
    QFETCH(QString,count);

    sendCommand(command,arg);
    if(!containTest.isEmpty()){
        QTRY_COMPARE(app.getCore()->realmComponent()->serverList().contains(containTest), result=="true");
    }
    if(!count.isEmpty()){
        QTRY_COMPARE(app.getCore()->realmComponent()->serverList().size(),count.toInt());
    }
}

void Tester::realmClientRegistered_data(){
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("arg");
    QTest::addColumn<QString>("containTest");
    QTest::addColumn<QString>("result");
    QTest::addColumn<QString>("count");

    loadTestData(5);
}
void Tester::realmClientRegistered(){
    QFETCH(QString,command);
    QFETCH(QString,arg);
    QFETCH(QString,containTest);
    QFETCH(QString,result);
    QFETCH(QString,count);

    sendCommand(command,arg);
    if(!containTest.isEmpty()){
        QTRY_COMPARE(app.getCore()->realmComponent()->clientList().contains(containTest), result=="true");
    }
    if(!count.isEmpty()){
        QTRY_COMPARE(app.getCore()->realmComponent()->clientList().size(),count.toInt());
    }
}

void Tester::serverStarted_data(){
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("result");

    loadTestData(2);
}
void Tester::serverStarted(){
    QFETCH(QString,command);
    QFETCH(QString,result);

    sendCommand(command);
    QTRY_COMPARE(app.getCore()->serverComponent()->isRunning(),result=="true");
}

QTEST_MAIN(Tester)
#include "main.moc"

