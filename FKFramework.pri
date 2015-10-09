CONFIG += c++14

CONFIG(debug, debug|release):{
    DEFINES+= FK_DEBUG
    DEFINES+= FK_NO_CONSTRUCTORS
}

DESTDIR =     $$OUT_PWD/..
LIBS +=     -L$$OUT_PWD/..
DEPENDPATH += $$OUT_PWD/..

#DEFINES += FK_NO_TODO

win32{
    CONFIG(release, debug|release){
        DLLDESTDIR = $$PWD/release/bin_win
    } else {
        DLLDESTDIR = $$PWD/debug/bin_win
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

INCLUDEPATH += $$PWD/FKUtility



