import QtQml 2.2
import FKApp 1.0

RoomEngine{
    id: roomEngine
    property Core core

    property UserInterface userInterface
    onMessageRequested: userInterface.showMessage(msg)

    property var serverAccess: Connections{
        target: roomEngine
        onInstructionDispatched: core.server.handleRoomInstruction(instruction)
    }
//    property var clientAccess:Connections{
//        target: roomEngine
//        onInstructionDispatched: core.client.handleRoomInstruction(instruction)
//    }
}
