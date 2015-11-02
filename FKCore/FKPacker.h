#ifndef FK_PACKER_H
#define FK_PACKER_H

#include <QString>

//Used to pack system objects to byte array for transferring via connectors

class QByteArray;
class FKSystemObject;

namespace FKPacker{
    QByteArray pack(QString MessageType, FKSystemObject* obj);
    QString unpack(QByteArray arr, FKSystemObject* &container);
    const QString event("ev");
    const QString basicEvent("be");
    const QString message("mg");
    const QString guestEvent("ge");
    const QString updateData("ud");
}

#endif // FKPACKER_H
