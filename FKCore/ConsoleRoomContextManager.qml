import QtQml 2.2
import FKApp 1.0

QtObject{
    property RoomEngine roomEngine
    property var userInterface: roomEngine.userContext.contextInterface
}
