TEMPLATE = subdirs

SUBDIRS += \
    FKCore \
\#    FKMultimedia \
\#    FKNetwork \
    FKLudo \
    FKRooms\FKChatRoom

HEADERS+= \
    FKTools/FKLogger.h \
    FKTools/FKOSType.h \
    FKTools/FKPostOperations.h \
    FKTools/FKCommands.h \
    FKTools/FKBasicEventSubjects.h \
    FKTools/FKAusviceIdentifiers.h
