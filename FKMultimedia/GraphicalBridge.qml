pragma Singleton
import QtQuick 2.3
import FKApp 1.0

UserInterfaceLayer{
    Core{

    }
    Component.onCompleted:{
        showMessage("Hello, world!")
    }
}
