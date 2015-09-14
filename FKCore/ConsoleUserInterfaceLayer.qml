import QtQml 2.0
import FKApp 1.0

UserInterface{
    property Core core

    onQuitApplicationRequested: core.quitApplication()
    onStartRealmRequested: core.startRealm()
    onStartClientInfrastructureRequested: core.startClientInfrastructure()
    onCreateUserRequested: core.createUserRecord(userName)
    onDeleteUserRequested: core.deleteUserRecord(userName)
    onClientUsersRequested: showMessage(core.userList())
    onRealmUsersRequested: showMessage(core.realmUserList())
    onClientLoginRequested: core.ausviseClientInfrastructure(clientName,password)
    onServerLoginRequested: core.ausviseServerInfrastructure(serverId,password)
    onCreateClientRequested: core.createClientRecord(clientName,password)
    onDeleteClientRequested: core.deleteClientRecord(clientName)
    onCreateServerRequested: core.createServerRecord(password)
    onDeleteServerRequested: core.deleteServerRecord(serverId)
    onStartServerInfrastructureRequested: core.startServer()
    onRegisterRoomTypeRequested: core.registerRoomType(roomType)
    onRegisterServerRoomTypeRequested: core.registerServerRoomType(roomType)
    onRemoveRoomTypeRequested: core.removeRoomType(roomType)
    onRemoveServerRoomTypeRequested: core.removeServerRoomType(roomType)
    onCreateRoomRequested: core.createRoomRequest(roomName,roomType,users)
    onCreateRoomRequestedRealm: core.createRoomRequestRealm(roomName,roomType)
    onJoinRoomRequested: core.joinRoomRequest(roomName,users)


    Component.onCompleted:{
            show()
    }
}
