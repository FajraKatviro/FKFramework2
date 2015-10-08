import QtQml 2.2
import FKApp 1.0

Core{
    property UserInterface userInterface

    onMessageRequested: ui.showMessage(msg)
    Component.onCompleted: initComponents()
}
