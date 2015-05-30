CONFIG += c++11

CONFIG(debug, debug|release):{
    DEFINES+= FK_DEBUG
    DEFINES+= FK_NO_CONSTRUCTORS
}

win32{
    CONFIG(release, debug|release){
        DESTDIR = ../../../fkbin/bin_win/rooms
        LIBS += -L../../../fkbin/bin_win
        DEPENDPATH += ../../../fkbin/bin_win
        DLLDESTDIR = ../../../release/bin_win/rooms
    } else {
        DESTDIR= ../../../fkbind/bin_win/rooms
        LIBS += -L../../../fkbind/bin_win
        DEPENDPATH += ../../../fkbind/bin_win
        DLLDESTDIR = ../../../debug/bin_win/rooms
    }
} else {
    mac {
        CONFIG(release, debug|release){
            DESTDIR = ../../../fkbin/bin_mac/rooms
            LIBS += -L../../../fkbin/bin_mac
            DEPENDPATH += ../../../fkbin/bin_mac
        } else {
            DESTDIR= ../../../fkbind/bin_mac/rooms
            LIBS += -L../../../fkbind/bin_mac
            DEPENDPATH += ../../../fkbind/bin_mac
        }
    } else {
        CONFIG(release, debug|release){
            DESTDIR = ../../../fkbin/bin_nix/rooms
            LIBS += -L../../../fkbin/bin_nix
            DEPENDPATH += ../../../fkbin/bin_nix
        } else {
            DESTDIR= ../../../fkbind/bin_nix/rooms
            LIBS += -L../../../fkbind/bin_nix
            DEPENDPATH += ../../../fkbind/bin_nix
        }
    }
}

INCLUDEPATH += $$PWD/FKTools $$PWD/FKCore



