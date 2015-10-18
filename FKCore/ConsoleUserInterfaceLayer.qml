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
    onShowServersRequested: {
        var serverList = core.realm.serverList(roomType);
        var total=serverList.length;
        if(total>0){
            var msg = (roomType == "") ?
                        qsTr("Servers registered in realm (%1 total):").arg(total) :
                        qsTr("Servers registered in realm for %2 room type (%1 total)").arg(total).arg(roomType);
            showMessage(msg);
            for(var i=0;i<total;++i){
                showMessage(serverList[i]);
            }
        }else{
            showMessage(qsTr("No servers found"));
        }
    }
    onShowClientsRequested: {
        var clientList = core.realm.clientList();
        var total=clientList.length;
        if(total>0){
            showMessage(qsTr("Clients registered in realm (%1 total):").arg(total));
            for(var i=0;i<total;++i){
                showMessage(clientList[i]);
            }
        }else{
            showMessage(qsTr("No client records found"));
        }
    }

    onStartServerInfrastructureRequested: core.startServerInfrastructure()
    onStopServerInfrastructureRequested: core.stopServerInfrastructure()
    onStartClientInfrastructureRequested: core.startClientInfrastructure()
    //onCreateUserRequested: core.createUserRecord(userName)
    //onDeleteUserRequested: core.deleteUserRecord(userName)
    //onClientUsersRequested: showMessage(core.userList())
    //onRealmUsersRequested: showMessage(core.realmUserList())
    //onClientLoginRequested: core.ausviseClientInfrastructure(clientName,password)
    //onServerLoginRequested: core.ausviseServerInfrastructure(serverId,password)
    onCreateClientRequested: core.realm.createClientRecord(clientName,password)
    onDeleteClientRequested: core.realm.deleteClientRecord(clientName)
    onCreateServerRequested: core.realm.createServerRecord(password)
    onDeleteServerRequested: core.realm.deleteServerRecord(serverId)
    //onRegisterServerRoomTypeRequested: core.registerServerRoomType(roomType)
    //onRemoveServerRoomTypeRequested: core.removeServerRoomType(roomType)
    //onCreateRoomRequested: core.createRoomRequest(roomName,roomType,users)
    //onCreateRoomRequestedRealm: core.createRoomRequestRealm(roomName,roomType)
    //onJoinRoomRequested: core.joinRoomRequest(roomName,users)

    welcomeString: qsTr("Welcome to FKApplication!")

    Component.onCompleted: show()
}
