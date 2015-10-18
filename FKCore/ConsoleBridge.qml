import QtQml 2.2
import FKApp 1.0

QtObject{
    property var ui: ConsoleUserInterfaceLayer{
        id: uiComponent
        core: coreComponent
    }

    property var core: CoreLayer{
        id: coreComponent
        userInterface: uiComponent
       // onRealmStarted: startServer()
       // onServerConnectedToRealm: ausviseServerInfrastructure(1,"pass")
    }

    property var roomEngine: RoomEngine{
        id: roomComponent
    }

    Component.onCompleted:{
         //   app.startRealm()
    }
}
