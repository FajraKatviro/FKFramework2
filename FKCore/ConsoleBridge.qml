//pragma Singleton
import QtQml 2.2
import FKApp 1.0

ConsoleUserInterfaceLayer{
    id: ui
    onQuitApplicationRequested: app.quitApplication()
    onStartRealmRequested: app.startRealm()
    onStartClientInfrastructureRequested: app.startClientInfrastructure()
    onCreateUserRequested: app.createUserRecord(userName)
    onDeleteUserRequested: app.deleteUserRecord(userName)
    onClientUsersRequested:{
        showMessage(app.userPool)
        showMessage(app.userList)
    }
    onRealmUsersRequested: showMessage(app.realmUserList())
    onClientLoginRequested: app.ausviseClientInfrastructure(clientName,password)
    onCreateClientRequested: app.createClientRecord(clientName,password)

    Core{
        id: app
        onMessageRequested: ui.showMessage(msg)
        onSystemMessageRequested: ui.showMessage("::"+msg)
    }
    Component.onCompleted:show()
}
