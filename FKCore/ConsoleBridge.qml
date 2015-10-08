import QtQuick 2.3
import FKApp 1.0

QtObject{
    default property var appComponents: [ ]

    ConsoleUserInterfaceLayer{
        id: uiLoader
        core: coreComponent
    }

    CoreLayer{
        id: coreComponent
        userInterface: uiLoader
       // onRealmStarted: startServer()
       // onServerConnectedToRealm: ausviseServerInfrastructure(1,"pass")
    }

    Component.onCompleted:{
         //   app.startRealm()
    }
}
