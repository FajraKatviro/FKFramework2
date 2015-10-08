import QtQml 2.0
import FKApp 1.0

Core{
    property UserInterface userInterface

    onMessageRequested: ui.showMessage(msg)
    Component.onCompleted: initComponents()
}
