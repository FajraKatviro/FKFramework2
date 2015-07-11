CONFIG += fkrooms
CONFIG += fkapp
CONFIG += fktools

TEMPLATE = subdirs

SUBDIRS += \
    FKCore \
#    FKMultimedia \
#    FKNetwork

HEADERS+= \
    FKUtility/FKLogger.h \
    FKUtility/FKOSType.h \
    FKUtility/FKPostOperations.h \
    FKUtility/FKCommands.h \
    FKUtility/FKBasicEventSubjects.h \
    FKUtility/FKAusviceIdentifiers.h

CONFIG(fkrooms){
    SUBDIRS += FKRooms
}

CONFIG(fkapp){
    SUBDIRS += FKLudo
}

CONFIG(fktools){
    SUBDIRS += FKTools
}
