import QtQuick
import QtQuick.Controls

import VoxelWorld

Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("Voxel World")

    property bool focused: false

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
        id: mouseArea
        anchors.fill: parent
        focus: true
        hoverEnabled: true
        cursorShape: root.focused ? Qt.BlankCursor : Qt.ArrowCursor

        onPressed: {
            if (!root.focused) {
                Helper.moveCursorToScreenCoords(mapToGlobal(Qt.point(root.width / 2, root.height / 2)));
                root.focused = true;
            }
        }

        onPositionChanged: mouse => {
            if (root.focused) {
                const currentX = mouse.x;
                const currentY = mouse.y;

                // Calculate delta movement
                const deltaX = currentX - root.width / 2;
                const deltaY = currentY - root.height / 2;

                // Send mouse movement to player controller
                engine.playerController.mouseMoved(deltaX, deltaY);

                Helper.moveCursorToScreenCoords(mapToGlobal(Qt.point(root.width / 2, root.height / 2)));
            }
        }

        Keys.onPressed: event => {
            if (event.key === Qt.Key_Escape && root.focused) {
                root.focused = false;
                Helper.moveCursorToScreenCoords(mapToGlobal(Qt.point(root.width / 2, root.height / 2)));
            } else {
                engine.playerController.keyPressed(event.key);
            }
        }
        Keys.onReleased: event => {
            engine.playerController.keyReleased(event.key);
        }
    }

    Text {
        id: debugText
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 6
        color: "white"
        font.pixelSize: 14

        Timer {
            interval: 100
            repeat: true
            running: true
            onTriggered: {
                const pos = engine.playerController.position;
                var text = `Player Position: (${pos.x.toFixed(2)}, ${pos.y.toFixed(2)}, ${pos.z.toFixed(2)})`;
                text += ` | Rotation: (${engine.playerController.rotation.x.toFixed(2)}, ${engine.playerController.rotation.y.toFixed(2)}, ${engine.playerController.rotation.z.toFixed(2)})`;
                debugText.text = text;
            }
        }
    }

    Button {
        id: exitButton
        text: "Exit"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 10

        visible: !root.focused

        onClicked: Qt.quit()
    }
}
