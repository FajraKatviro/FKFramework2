#CONFIG += fkrooms
CONFIG += fkapp
CONFIG += fktools
CONFIG += fktests

TEMPLATE = subdirs

SUBDIRS += \
    FKCore \
#    FKMultimedia \
#    FKNetwork

HEADERS+= \
    FKUtility/FKLogger.h \
    FKUtility/FKOSType.h \
    FKUtility/FKCommands.h

CONFIG(fkrooms){
    SUBDIRS += FKRooms
}

CONFIG(fkapp){
    SUBDIRS += FKLudo
}

CONFIG(fktools){
    SUBDIRS += FKTools
}

CONFIG(fktests){
    SUBDIRS += FKTests
}
