#-------------------------------------------------
#
# Project created by QtCreator 2015-05-28T11:21:05
#
#-------------------------------------------------

QT       -= gui

TARGET = FKChatRoom
TEMPLATE = lib
CONFIG += dll

DEFINES += FKCHATROOM_LIBRARY

SOURCES += FKChatRoom.cpp

HEADERS += FKChatRoom.h\
        fkchatroom_global.h

include(../../FKRoomPlugin.pri)
