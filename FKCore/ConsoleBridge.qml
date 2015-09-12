import QtQuick 2.3
import FKApp 1.0

Item{
    Loader{
        id: uiLoader
        source: coreComponent.uiSource === "" ? "ConsoleUserInterfaceLayer.qml" : coreComponent.uiSource
    }

    CoreLayer{
        id: coreComponent
       // onRealmStarted: startServer()
       // onServerConnectedToRealm: ausviseServerInfrastructure(1,"pass")
    }

    Binding{
        target: coreComponent
        property: 'userInterface'
        value: uiLoader.item
        when: uiLoader.status == Loader.Ready
    }

    Binding{
        target: uiLoader.item
        property: 'core'
        value: coreComponent
        when: uiLoader.status == Loader.Ready
    }

    Component.onCompleted:{
         //   app.startRealm()
    }
}
