import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import cs2server 1.0

RowLayout {
    id: panel
    property bool isRunning: ProcessHandler.isRunning

    Button {
        id: selectButton
        text: "Select Executable"
        onClicked: fileDialog.open()
        Layout.alignment: Qt.AlignLeft

        FileDialog {
            id: fileDialog
            title: "Please choose an executable file"
            options: FileDialog.ReadOnly
            nameFilters: ["Executable files (*.exe)"]

            onAccepted: {
                selectedFileLabel.text = fileDialog.selectedFile
                ProcessHandler.setFilePath(fileDialog.selectedFile)
                console.log("You chose: " + fileDialog.selectedFile)
            }
            onRejected: {
                console.log("Canceled")
            }
        }
    }

    Rectangle {
        width: selectButton.width * 2
        height: startButton.height
        radius: 10
        clip: true

        Layout.alignment: Qt.AlignLeft

        Label {
            id: selectedFileLabel
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                ToolTip.visible: selectedFileLabel.text.length ? containsMouse : false
                ToolTip.text: selectedFileLabel.text
            }
        }
    }

    // A spacer.
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
