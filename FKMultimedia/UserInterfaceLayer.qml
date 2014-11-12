import QtQuick 2.3

UserInterface{

    onShowMessageRequested:{
        txt.text=msg
    }

    Item{
        id:viewRoot
        Rectangle{
            id:menu
            anchors.fill: parent
            color: "black"
            Text{
                id:txt
                anchors.fill: parent
                color: "white"
            }
        }
        Item{
            id:room
            anchors.fill: parent
//            Loader{

//            }
        }
    }
}
