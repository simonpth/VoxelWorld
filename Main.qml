import QtQuick
import QtQuick.Controls

import VoxelWorld

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Voxel World")

    GLQuickItem {
        id: glItem
        anchors.fill: parent
    }
}
