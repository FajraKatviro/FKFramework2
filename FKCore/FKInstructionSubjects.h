#ifndef FKINSTRUCTIONSUBJECTS_H
#define FKINSTRUCTIONSUBJECTS_H

//Identifiers used for FKInstructionObject to determine type of order

namespace FKInstructionSubject{
    const qint32 loadModule         = 1;
    const qint32 releaseModule      = 2;
    const qint32 createContext      = 3;
    const qint32 releaseContext     = 4;
    const qint32 createObject       = 5;
    const qint32 deleteObject       = 6;
    const qint32 addClient          = 7;
    const qint32 removeClient       = 8;
}

#endif // FKINSTRUCTIONSUBJECTS_H
