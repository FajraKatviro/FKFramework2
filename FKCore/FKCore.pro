QT += qml
CONFIG += dll

TARGET = FKCore
TEMPLATE = lib

DEFINES += FKCORE_LIBRARY

SOURCES += \
    FKApplication.cpp \
    FKUserInterface.cpp \
    FKSimpleCore.cpp \
    FKObjectManager.cpp \
    FKDataBase.cpp \
    FKSystemObject.cpp \
    FKMessage.cpp \
    FKBasicEvent.cpp \
    FKEventObject.cpp \
    FKObject.cpp \
    FKFSDB.cpp \
    FKConsoleInterface.cpp \
    \ #FKLocalCore.cpp \
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
    FKClientInfrastructureReferent.cpp \
    FKServerConnectionManager.cpp \
    FKUsersStructure.cpp \
    FKUser.cpp \
    FKObjectPool.cpp \
    FKCommandResolver.cpp \
    FKUserInfrastructure.cpp \
    FKUpdateData.cpp \
    FKUpdateChannel.cpp \
    FKVersionList.cpp \
    FKPathResolver.cpp \
    FKRoomInfrastructure.cpp \
    FKRoomEngine.cpp \
    FKRoomContext.cpp \
    FKThreadedComponent.cpp

HEADERS += \
    fkcore_global.h \
    FKApplication.h \
    FKUserInterface.h \
    FKSimpleCore.h \
    FKObjectManager.h \
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
    \ #FKLocalCore.h \
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
    FKClientInfrastructureReferent.h \
    FKServerConnectionManager.h \
    FKUsersStructure.h \
    FKUser.h \
    FKObjectPool.h \
    FKService.h \
    FKCommandResolver.h \
    FKUserInfrastructure.h \
    FKUpdateData.h \
    FKUpdateChannel.h \
    FKUpdateChannelStatus.h \
    FKVersionList.h \
    FKPathResolver.h \
    FKRoomInfrastructure.h \
    FKRoomEngine.h \
    FKRoomContext.h \
    FKThreadedComponent.h

include(../FKFramework.pri)
include(fkservice/FKCore_service.pri)

RESOURCES += \
    FKCore.qrc
