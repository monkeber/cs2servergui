import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts
import cs2server 1.0

RowLayout {
    id: panel
    property bool isRunning: ProcessHandler.isRunning

    Item {
        Layout.fillWidth: true
    }

    Button {
        id: startButton
        text: "Start"
        onClicked: ProcessHandler.start()
        Layout.alignment: Qt.AlignRight
    }
    Rectangle {
        width: startButton.width
        height: startButton.height
        radius: 10

        Layout.alignment: Qt.AlignRight

        Label {
            id: statusLabel
            anchors.centerIn: parent

            text: "Stopped"
        }
    }

    Button {
        id: stopButton
        text: "Stop"
        onClicked: ProcessHandler.stop()
        Layout.alignment: Qt.AlignRight
    }

    states: [
        State {
            name: "running"
            when: panel.isRunning
            PropertyChanges {
                target: startButton
                down: true
            }
            PropertyChanges {
                target: statusLabel
                text: "Running..."
            }
        },
        State {
            name: "stopped"
            when: !panel.isRunning
            PropertyChanges {
                target: stopButton
                down: true
            }
        }
    ]
}
