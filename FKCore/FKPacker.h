#ifndef FK_PACKER_H
#define FK_PACKER_H

#include <QString>

class QByteArray;
class FKSystemObject;

namespace FKPacker{
    QByteArray pack(QString MessageType, FKSystemObject* obj);
    QString unpack(QByteArray arr, FKSystemObject* &container);
    const QString event("ev");
    const QString basicEvent("be");
    const QString message("mg");
    const QString guestEvent("ge");
}

#endif // FKPACKER_H
