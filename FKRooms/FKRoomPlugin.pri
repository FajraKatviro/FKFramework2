CONFIG += c++14

CONFIG(debug, debug|release):{
    DEFINES+= FK_DEBUG
    DEFINES+= FK_NO_CONSTRUCTORS
}

DESTDIR=      $$OUT_PWD/../../rooms
LIBS+=      -L$$OUT_PWD/../..
DEPENDPATH+=  $$OUT_PWD/../..

win32{
    CONFIG(release, debug|release){
        DLLDESTDIR = $$PWD/../release/rooms/bin_win
    } else {
        DLLDESTDIR = $$PWD/../debug/rooms/bin_win
    }
} else {
    mac {
        CONFIG(release, debug|release){
        } else {
        }
    } else {
        CONFIG(release, debug|release){
        } else {
        }
    }
}

LIBS += -lFKCore
INCLUDEPATH += $$PWD/FKUtility $$PWD/FKCore



