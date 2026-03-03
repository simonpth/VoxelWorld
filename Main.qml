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

    MouseArea {
        anchors.fill: parent
        focus: true

        Keys.onPressed: (event) => {
            engine.playerController.keyPressed(event.key);
        }
        Keys.onReleased: (event) => {
            engine.playerController.keyReleased(event.key);
        }

        onPositionChanged: (event) => {
            engine.playerController.mouseMoved(event.x, event.y);
            Helper.moveCursorToCenter();
        }
    }
}
