import QtQuick
import QtQuick.Controls.Basic

import VoxelWorld

Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("Voxel World")

    color: "gray"

    Button {
        id: startButton
        text: qsTr("Start Game")
        anchors.centerIn: parent
        onClicked: {
            startButton.visible = false;
            gameWindowLoader.active = true;
        }
    }

    Loader {
        id: gameWindowLoader
        anchors.fill: parent
        active: false
        sourceComponent: Component {
            GameWindow {
                id: gameWindow
                anchors.fill: parent
            }
        }
    }
}
