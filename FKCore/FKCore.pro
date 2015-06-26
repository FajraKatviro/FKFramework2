QT += qml
CONFIG += dll

TARGET = FKCore
TEMPLATE = lib

DEFINES += FKCORE_LIBRARY

SOURCES += \
    FKApplication.cpp \
    FKUserInterface.cpp \
    FKAbstractCore.cpp \
    FKObjectManager.cpp \
    FKFilesList.cpp \
    FKDataBase.cpp \
    FKSystemObject.cpp \
    FKMessage.cpp \
    FKBasicEvent.cpp \
    FKEventObject.cpp \
    FKObject.cpp \
    FKFSDB.cpp \
    FKConsoleInterface.cpp \
    FKLocalCore.cpp \
    FKConsoleReader.cpp \
    FKPacker.cpp \
    FKConnector.cpp \
    FKLocalConnector.cpp \
    FKRealm.cpp \
    FKConnectionManager.cpp \
    FKRealmConnectionManager.cpp \
    FKClientInfrastructure.cpp \
    FKClientInfrastructureConnectionManager.cpp \
    FKInfrastructure.cpp \
    FKRoomData.cpp \
    FKRoomDataFilter.cpp \
    FKDataBasePath.cpp \
    FKRealmDBMap.cpp \
    FKAusviceData.cpp \
    FKRoomModule.cpp \
    FKPackage.cpp \
    FKPackageManager.cpp \
    FKRoomInviteData.cpp \
    FKRoom.cpp \
    FKServerInfrastructure.cpp \
    FKUserInfrastructureSlot.cpp \
    FKServerConnectionManager.cpp \
    FKUsersStructure.cpp \
    FKUser.cpp \
    FKObjectPool.cpp

HEADERS += \
    fkcore_global.h \
    FKApplication.h \
    FKUserInterface.h \
    FKAbstractCore.h \
    FKObjectManager.h \
    FKFilesList.h \
    FKDataBase.h \
    FKIDGenerator.h \
    FKSystemObject.h \
    FKFactory.h \
    FKMessage.h \
    FKBasicEvent.h \
    FKEventObject.h \
    FKObject.h \
    FKFSDB.h \
    FKConsoleInterface.h \
    FKLocalCore.h \
    FKConsoleReader.h \
    FKPacker.h \
    FKConnector.h \
    FKLocalConnector.h \
    FKRealm.h \
    FKConnectionManager.h \
    FKRealmConnectionManager.h \
    FKClientInfrastructure.h \
    FKClientInfrastructureConnectionManager.h \
    FKInfrastructure.h \
    FKRoomData.h \
    FKRoomDataFilter.h \
    FKDataBasePath.h \
    FKRealmDBMap.h \
    FKAusviceData.h \
    FKRoomModule.h \
    FKRoomLoader.h \
    FKPackage.h \
    FKPackageManager.h \
    FKRoomComponents.h \
    FKRoomInviteData.h \
    FKRoom.h \
    FKServerInfrastructure.h \
    FKUserInfrastructureSlot.h \
    FKServerConnectionManager.h \
    FKUsersStructure.h \
    FKUser.h \
    FKObjectPool.h \
    FKService.h

include(../FKFramework.pri)
include(fkservice/FKCore_service.pri)

RESOURCES += \
    FKCore.qrc
