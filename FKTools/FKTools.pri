CONFIG += c++11

CONFIG(debug, debug|release):{
    DEFINES+= FK_DEBUG
    DEFINES+= FK_NO_CONSTRUCTORS
}

DESTDIR = $$PWD/../tools

INCLUDEPATH += $$PWD/../FKUtility



