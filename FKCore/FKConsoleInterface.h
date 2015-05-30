#ifndef FKCONSOLEINTERFACE_H
#define FKCONSOLEINTERFACE_H

#include "FKUserInterface.h"

#include <QThread>
#include <QTimer>
#include <QStringList>

class FKConsoleReader;

class FKCORESHARED_EXPORT FKConsoleInterface : public FKUserInterface{
    Q_OBJECT
public:
    FKConsoleInterface(QObject* parent=0);
    ~FKConsoleInterface();
signals:
    void clientUsersRequested();
    void realmUsersRequested();
public slots:
    void show();
    void showMessage(const QString& m);
private slots:
    void processInput(QString input);
    void autoInput();
private:
    QString welcomeString()const;
    void printHelp();
    static bool isCommand(QString& arg, const QString& command);
    static bool hasKey(QString& arg, const QString& key);
    void showUsers(QString& arg);
    void loginClient(const QString& arg);
    void loginServer(const QString& arg);
    void createClient(const QString& arg);
    void createServer(QString arg);
    void createRoom(QString arg);
    void registerRoomType(QString arg);
    QThread _consoleReaderThread;
    FKConsoleReader* _consoleReader;
    QStringList _autoCommands;
    QTimer _autoInputTimer;
};

#endif // FKCONSOLEINTERFACE_H
