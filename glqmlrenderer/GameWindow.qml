import QtQuick
import QtQuick.Controls

import VoxelWorld

Item {
    id: root
    property bool focused: false
    property var engine

    GLQuickItem {
        id: glItem
        anchors.fill: parent
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: root.focused ? Qt.BlankCursor : Qt.ArrowCursor

        onPressed: {
            if (!root.focused) {
                Helper.moveCursorToScreenCoords(mapToGlobal(Qt.point(root.width / 2, root.height / 2)));
                root.focused = true;
                mouseArea.forceActiveFocus();
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
                if (root.engine)
                    root.engine.playerController.mouseMoved(deltaX, deltaY);
                else {
                    root.engine = EngineQML.instance();
                }

                Helper.moveCursorToScreenCoords(mapToGlobal(Qt.point(root.width / 2, root.height / 2)));
            }
        }

        Keys.onPressed: event => {
            if (event.key === Qt.Key_Escape && root.focused) {
                root.focused = false;
                Helper.moveCursorToScreenCoords(mapToGlobal(Qt.point(root.width / 2, root.height / 2)));
            } else {
                if (root.engine)
                    root.engine.playerController.keyPressed(event.key);
                else
                    root.engine = EngineQML.instance();
            }
        }
        Keys.onReleased: event => {
            root.engine.playerController.keyReleased(event.key);
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
                if (root.engine) {
                    const pos = root.engine.playerController.position;
                    var text = `Player Position: (${pos.x.toFixed(2)}, ${pos.y.toFixed(2)}, ${pos.z.toFixed(2)})`;
                    text += ` | Rotation: (${root.engine.playerController.rotation.x.toFixed(2)}, ${root.engine.playerController.rotation.y.toFixed(2)}, ${root.engine.playerController.rotation.z.toFixed(2)})`;
                    text += "\n";
                    text += `FPS: ${glItem.renderer ? glItem.renderer.fps : "N/A"}`;
                    debugText.text = text;
                }
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
