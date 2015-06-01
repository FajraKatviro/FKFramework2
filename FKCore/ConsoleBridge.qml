//pragma Singleton
import QtQml 2.2
import FKApp 1.0

ConsoleUserInterfaceLayer{
    id: ui

    CoreLayer{
        id: app
        userInterface: ui

        onRealmStarted: startServer()
        onServerConnectedToRealm: ausviseServerInfrastructure(1,"pass")
    }

    Component.onCompleted:{
            app.startRealm()
    }
}
