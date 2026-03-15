import QtQuick

import VoxelWorld

Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("Voxel World")

    GameWindow {
        id: gameWindow
        anchors.fill: parent
    }
}
