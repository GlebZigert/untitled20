import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 1.4 as QQC1
Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Column{
    id: column
    anchors.fill: parent
    width: parent.width
    height: parent.height
    Rectangle{

    id: video
    width: parent.width
    height: parent.height-20

color:"green"
    }

    Row{

    id: panel
        width: parent.width
    height: 20



    TextInput{
    id: path
    height: parent.height
    width: 580

    }

    QQC1.Button{

    height: parent.height
    width: 20

    }

    QQC1.Button{

    height: parent.height
    width: 20

    }

    QQC1.Button{

    height: parent.height
    width: 20

    }
    }

    }




}
