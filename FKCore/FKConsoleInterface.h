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
    virtual void show()override;
    virtual void showMessage(const QString& m)override;
    virtual void showUpdateData()override;
private:
    QThread _consoleReaderThread;
    FKConsoleReader* _consoleReader;
};

#endif // FKCONSOLEINTERFACE_H
