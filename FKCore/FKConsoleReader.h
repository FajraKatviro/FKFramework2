#ifndef FKCONSOLEREADER_H
#define FKCONSOLEREADER_H

#include <QObject>
#include <QSemaphore>

class FKConsoleReader : public QObject{
    Q_OBJECT
public:
    explicit FKConsoleReader(QObject *parent = 0);
    ~FKConsoleReader();
public slots:
    void setEnabled(const bool enabled=true);
signals:
    void gotLine(const QString line);
private slots:
    void tryRead();
private:
    QSemaphore _semaphore;
};

#endif // FKCONSOLEREADER_H
