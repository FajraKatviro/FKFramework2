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
    onClientUsersRequested: {showMessage(app.userPool); showMessage(app.userList)}
    onRealmUsersRequested: showMessage(app.realmUserList())
    onClientLoginRequested: app.ausviseClientInfrastructure(clientName,password)
    onServerLoginRequested: app.ausviseServerInfrastructure(serverId,password)
    onCreateClientRequested: app.createClientRecord(clientName,password)
    onCreateServerRequested: app.createServerRecord(password)
    onStartServerInfrastructureRequested: app.startServer()
    onRegisterRoomTypeRequested: app.registerRoomType(roomType)
    onRegisterServerRoomTypeRequested: app.registerServerRoomType(roomType)
    onCreateRoomRequested: app.createRoomRequest(roomName,roomType)
    onCreateRoomRequestedRealm: app.createRoomRequestRealm(roomName,roomType)
    onCreateCustomServerRequested: app.createCustomServerRequest()

    Core{
        id: app
        onMessageRequested: ui.showMessage(msg)
        onSystemMessageRequested: ui.showMessage("::"+msg)
        onClientLoggedIn: {}
    }
    Component.onCompleted:show()
}
