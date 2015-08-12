#ifndef FKCONSOLEINTERFACE_H
#define FKCONSOLEINTERFACE_H

#include "FKUserInterface.h"

#include <QThread>
#include <QStringList>

class FKConsoleReader;

class FKCORESHARED_EXPORT FKConsoleInterface : public FKUserInterface{
    Q_OBJECT
public:
    FKConsoleInterface(QObject* parent=0);
    ~FKConsoleInterface();
public slots:
    void show();
    void showMessage(const QString& m);
private:
    QThread _consoleReaderThread;
    FKConsoleReader* _consoleReader;
};

#endif // FKCONSOLEINTERFACE_H
