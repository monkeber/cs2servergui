import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts
import cs2server 1.0

RowLayout {
    id: panel
    property bool isRunning: ProcessHandler.isRunning

    Button {
        id: startButton
        text: "Start"
        onClicked: ProcessHandler.start()
        Layout.alignment: Qt.AlignRight
    }
    Label {
        id: statusLabel
        text: "Stopped"
        Layout.alignment: Qt.AlignHCenter
    }
    Button {
        id: stopButton
        text: "Stop"
        onClicked: ProcessHandler.stop()
        Layout.alignment: Qt.AlignLeft
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
