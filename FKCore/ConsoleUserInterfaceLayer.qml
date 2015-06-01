import QtQml 2.0
import FKApp 1.0

UserInterface{
    default property Core core

    onQuitApplicationRequested: core.quitApplication()
    onStartRealmRequested: core.startRealm()
    onStartClientInfrastructureRequested: core.startClientInfrastructure()
    onCreateUserRequested: core.createUserRecord(userName)
    onDeleteUserRequested: core.deleteUserRecord(userName)
    onSelectUserRequested: core.startUser(userName)
    onDeselectUserRequested: core.stopUser(userName)
    onClientUsersRequested: {showMessage(core.userPool); showMessage(core.userList)}
    onRealmUsersRequested: showMessage(core.realmUserList())
    onClientLoginRequested: core.ausviseClientInfrastructure(clientName,password)
    onServerLoginRequested: core.ausviseServerInfrastructure(serverId,password)
    onCreateClientRequested: core.createClientRecord(clientName,password)
    onCreateServerRequested: core.createServerRecord(password)
    onStartServerInfrastructureRequested: core.startServer()
    onRegisterRoomTypeRequested: core.registerRoomType(roomType)
    onRegisterServerRoomTypeRequested: core.registerServerRoomType(roomType)
    onCreateRoomRequested: core.createRoomRequest(roomName,roomType)
    onCreateRoomRequestedRealm: core.createRoomRequestRealm(roomName,roomType)
    onCreateCustomServerRequested: core.createCustomServerRequest()


    Component.onCompleted:{
            show()
    }
}
