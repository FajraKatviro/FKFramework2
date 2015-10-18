QT += core qml
QT -= gui
CONFIG += console

TARGET = ludo
TEMPLATE = app

SOURCES += \
    main.cpp

LIBS += -lFKCore #-lFKMultimedia
INCLUDEPATH += $$PWD/../FKCore #$$PWD/../FKMultimedia

include(../FKFramework.pri)

macx{CONFIG -= app_bundle}
