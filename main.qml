import QtQuick 2.6
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.1


import org.freedesktop.gstreamer.GLVideoItem 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 480
    height: 800
    x: 10
    y: 10
    color: "black"

        Rectangle {
            color: Qt.rgba(1, 1, 1, 0.7)
            border.width: 10
            border.color: "white"
            width : parent.width - 20
            height: parent.width - 20
            radius: 8

            GstGLVideoItem {
                id: video
                objectName: "videoItem"
                anchors.centerIn: parent
                width: parent.width
                height: parent.height
                visible: true

                signal signal_takePicture();
            }



            Keys.onPressed: {
                if(event.key === Qt.Key_Return || event.key === Qt.Key_Enter){
                    signal_takePicture();
                }
            }

            MouseArea {
                id: mousearea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    video.signal_takePicture();
                }
            }
        }
}
