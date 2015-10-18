QT += testlib qml xml

mac {
    CONFIG += c++11
    QMAKE_CXXFLAGS_CXX11 = -std=c++14 -stdlib=libc++
} else {
    CONFIG += c++14
}

CONFIG(debug, debug|release):{
    DEFINES+= FK_DEBUG
    DEFINES+= FK_NO_CONSTRUCTORS
}

DESTDIR =     $$PWD/../tests/bin
LIBS+=      -L$$OUT_PWD/../..
DEPENDPATH+=  $$OUT_PWD/../..

INCLUDEPATH += $$PWD/../FKUtility $$PWD
INCLUDEPATH += $$PWD/../FKCore

LIBS += -lFKCore

HEADERS += \
    $$PWD/FKConsoleTester.h

