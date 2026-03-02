import QtQuick
import QtQuick.Controls

import VoxelWorld

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Voxel World")

    Engine {
        id: engine
    }

    GLQuickItem {
        id: glItem
        anchors.fill: parent

        onRendererChanged: {
            if (renderer) {
                renderer.engine = engine;
            }
        }
    }
}
