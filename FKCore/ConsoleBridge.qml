import QtQml 2.2
import FKApp 1.0

QtObject{
    property var ui: ConsoleUserInterfaceLayer{
        id: uiComponent
        core: coreComponent
        roomEngine: roomComponent
    }

    property var core: CoreLayer{
        id: coreComponent
        userInterface: uiComponent
        roomEngine: roomComponent
       // onRealmStarted: startServer()
       // onServerConnectedToRealm: ausviseServerInfrastructure(1,"pass")
    }

    property var roomEngine: RoomEngineLayer{
        id: roomComponent
        core: coreComponent
        userInterface: uiComponent
    }

    Component.onCompleted:{
         //   core.startRealm()
    }
}
