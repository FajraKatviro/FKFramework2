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
            target.path = $$PWD/release/bin_mac
        } else {
            target.path = $$PWD/debug/bin_mac
        }
    } else {
        CONFIG(release, debug|release){
            target.path = $$PWD/release/bin_nix
        } else {
            target.path = $$PWD/debug/bin_nix
        }
    }
    INSTALLS += target
}

INCLUDEPATH += $$PWD/FKUtility



