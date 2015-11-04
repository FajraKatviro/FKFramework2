import QtQml 2.2
import FKApp 1.0

Core{
    id: core
    property UserInterface userInterface
    property RoomEngine roomEngine

    onMessageRequested: userInterface.showMessage(msg)

    property var roomEngineserverAccess: Connections{
        target: core.server
        onRoomInstruction: roomEngine.processInstruction(instruction)
    }

    Component.onCompleted: initComponents()
}
