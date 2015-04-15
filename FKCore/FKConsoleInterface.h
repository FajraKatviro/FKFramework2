#ifndef FKCONSOLEINTERFACE_H
#define FKCONSOLEINTERFACE_H

#include "FKUserInterface.h"

#include <QThread>

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
};

#endif // FKCONSOLEINTERFACE_H
