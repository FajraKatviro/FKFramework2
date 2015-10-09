QT += testlib qml
CONFIG += c++14

CONFIG(debug, debug|release):{
    DEFINES+= FK_DEBUG
    DEFINES+= FK_NO_CONSTRUCTORS
}

DESTDIR = $$PWD/../tests
LIBS+=      -L$$OUT_PWD/../..
DEPENDPATH+=  $$OUT_PWD/../..

INCLUDEPATH += $$PWD/../FKUtility
INCLUDEPATH += $$PWD/../FKCore

LIBS += -lFKCore

