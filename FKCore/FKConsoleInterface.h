#ifndef FKCONSOLEINTERFACE_H
#define FKCONSOLEINTERFACE_H

#include "FKUserInterface.h"

#include <QThread>
#include <QStringList>

class FKConsoleReader;

class FKCORESHARED_EXPORT FKConsoleInterface : public FKUserInterface{
    Q_OBJECT
    Q_PROPERTY(QString welcomeString READ welcomeString WRITE setWelcomeString NOTIFY welcomeStringChanged)
public:
    FKConsoleInterface(QObject* parent=0);
    ~FKConsoleInterface();
    void setWelcomeString(const QString arg);
public slots:
    virtual void show()override;
    virtual void showMessage(const QString& m)override;
    virtual void showUpdateData()override;
    QString welcomeString()const;
signals:
    void welcomeStringChanged();
private:
    QThread _consoleReaderThread;
    FKConsoleReader* _consoleReader;
    QString _welcomeString;
};

#endif // FKCONSOLEINTERFACE_H
