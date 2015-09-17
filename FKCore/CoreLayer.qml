import QtQml 2.0
import FKApp 1.0

Core{
    property UserInterface userInterface

    onMessageRequested: ui.showMessage(msg)
    onSystemMessageRequested: ui.showMessage("::"+msg)
    onClientLoggedIn: {}
    onCustomServerReady: ui.showMessage(qsTr("Custom server sucessfully started"))
    onClientGotUpdateList: ui.showUpdateData()
}
