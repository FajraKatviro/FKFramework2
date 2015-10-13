import QtQml 2.2
import FKApp 1.0

UserInterface{
    property Core core

    onQuitApplicationRequested: core.quitApplication()
    onStartRealmRequested: core.startRealmInfrastructure()
    onStopRealmRequested: core.stopRealmInfrastructure()
    onRegisterRoomTypeRequested: core.realm.registerRoomType(roomType)
    onRemoveRoomTypeRequested: core.realm.removeRoomType(roomType)
    onShowRoomTypesRequested: {
        var roomTypeList = core.realm.roomTypeList(serverId);
        var total=roomTypeList.length;
        if(total>0){
            showMessage(qsTr("Room types registered in realm (%1 total):").arg(total));
            for(var i=0;i<total;++i){
                showMessage(roomTypeList[i]);
            }
        }else{
            showMessage(qsTr("No room types found"));
        }
    }

    onStartServerInfrastructureRequested: core.startServerInfrastructure()
    onStartClientInfrastructureRequested: core.startClientInfrastructure()
    //onCreateUserRequested: core.createUserRecord(userName)
    //onDeleteUserRequested: core.deleteUserRecord(userName)
    //onClientUsersRequested: showMessage(core.userList())
    //onRealmUsersRequested: showMessage(core.realmUserList())
    //onClientLoginRequested: core.ausviseClientInfrastructure(clientName,password)
    //onServerLoginRequested: core.ausviseServerInfrastructure(serverId,password)
    //onCreateClientRequested: core.createClientRecord(clientName,password)
    //onDeleteClientRequested: core.deleteClientRecord(clientName)
    //onCreateServerRequested: core.createServerRecord(password)
    //onDeleteServerRequested: core.deleteServerRecord(serverId)
    //onRegisterServerRoomTypeRequested: core.registerServerRoomType(roomType)
    //onRemoveServerRoomTypeRequested: core.removeServerRoomType(roomType)
    //onCreateRoomRequested: core.createRoomRequest(roomName,roomType,users)
    //onCreateRoomRequestedRealm: core.createRoomRequestRealm(roomName,roomType)
    //onJoinRoomRequested: core.joinRoomRequest(roomName,users)

    welcomeString: qsTr("Welcome to FKApplication!")

    Component.onCompleted: show()
}
