import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import application 1.0

GridLayout {
    id: panel

    property bool isRunning: AppData.serverProcess.isRunning

    rows: 1
    columns: 7

    Button {
        id: selectButton

        Layout.fillHeight: true
        Layout.alignment: Qt.AlignLeft

        text: qsTr("Select Executable")
        font: Globals.font
        onClicked: fileDialog.open()

        FileDialog {
            id: fileDialog
            title: qsTr("Please select an executable file")
            options: FileDialog.ReadOnly
            nameFilters: ["Executable files (*.exe)"]

            onAccepted: {
                selectedFileLabel.text = fileDialog.selectedFile
                AppData.settings.setExecutablePath(fileDialog.selectedFile)
                console.log("You selected: " + fileDialog.selectedFile)
            }
        }
    }

    Rectangle {
        Layout.columnSpan: 2
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignLeft

        radius: 10
        clip: true

        ToolTip {
            parent: parent
            visible: selectedFileLabel.text.length ? mouseArea.containsMouse : false
            text: selectedFileLabel.text
            font: Globals.font
        }

        Label {
            id: selectedFileLabel
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: AppData.settings.executablePath
            font: Globals.font
        }
        MouseArea {
            id: mouseArea

            anchors.fill: parent
            hoverEnabled: true
        }
    }

    // A spacer.
    Item {
        Layout.fillWidth: true
    }

    Button {
        id: startButton

        Layout.alignment: Qt.AlignRight

        text: qsTr("Start")
        font: Globals.font
        onClicked: AppData.serverProcess.start()
    }
    Rectangle {
        Layout.alignment: Qt.AlignRight
        Layout.preferredWidth: startButton.width
        Layout.fillHeight: true

        radius: 10

        Label {
            id: statusLabel

            anchors.centerIn: parent

            text: qsTr("Stopped")
            font: Globals.font
        }
    }

    Button {
        id: stopButton

        Layout.alignment: Qt.AlignRight

        text: qsTr("Stop")
        font: Globals.font

        onClicked: AppData.serverProcess.stop()
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
                text: qsTr("Running...")
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
