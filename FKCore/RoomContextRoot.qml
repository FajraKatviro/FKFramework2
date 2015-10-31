import QtQml 2.2
import FKApp 1.0

QtObject{
    property RoomEngine roomEngine
    readonly property var contexts: roomEngine.contexts
    function doLog(){
        console.log("alive!")
    }
}
