import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 1.4 as QQC1
import VideoPlayer 1.0
import MyQMLEnums 13.37
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
    VideoPlayer{

    id: player
    width: parent.width
    height: parent.height-20


    }

    Row{

    id: panel
        width: parent.width
    height: 20



    TextInput{
    id: path
    text: "/home/master/1.mp4"
    height: parent.height
    width: 580

    }

    QQC1.Button{

    height: parent.height
    width: 20
    onClicked: {
        player.source=path.text
        player.start(StreamType.Streaming)
    }

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
