#include "FKConsoleReader.h"

#include <QTextStream>
#include <QTimer>

#include "FKLogger.h"

/*!
\class FKConsoleReader
\brief This class used for non-blocking reading from stdin. Class instance should be placed in a separate thread.
*/

/*!
 * \brief Creates instance of object. Note that reading would not be happend before setEnabled() call
 */

FKConsoleReader::FKConsoleReader(QObject *parent):QObject(parent),_semaphore(0){
    FK_CBEGIN
    const qint32 readFrequency=200;
    QTimer* t=new QTimer(this);
    connect(t,SIGNAL(timeout()),SLOT(tryRead()));
    t->start(readFrequency);
    FK_CEND
}

/*!
 * \brief Destructs object
 */

FKConsoleReader::~FKConsoleReader(){
    FK_DBEGIN
    FK_DEND
}

/*!
 * \brief If enabled, start reading from console stdin. Note that you should disable reading and get last command before you can quit thread contains this object
 */

void FKConsoleReader::setEnabled(const bool enabled){
    if(enabled && !_semaphore.available()){
        _semaphore.release();
    }else if(!enabled && _semaphore.available()){
        _semaphore.acquire();
    }
}

/*!
 * \fn void FKConsoleReader::gotLine(const QString line)
 * \brief Signal emitted when \i line was readed from stdin
 */

void FKConsoleReader::tryRead(){
    if(_semaphore.available()){
        QTextStream in(stdin,QIODevice::ReadOnly);
        const QString input=in.readLine();
        emit gotLine(input);
    }
}
