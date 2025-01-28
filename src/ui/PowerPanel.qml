import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import application 1.0

GridLayout {
    id: panel

    property bool isRunning: AppData.serverProcess.isRunning
    implicitHeight: Math.max(fileRect.implicitHeight, statusRect.implicitHeight)

    rows: 1
    columns: 7

    Button {
        id: selectButton

        Layout.fillHeight: true
        Layout.alignment: Qt.AlignLeft

        text: qsTr("Select Executable")
        font: Globals.font
        onClicked: fileDialog.open()

        Material.background: Theme.primary

        FileDialog {
            id: fileDialog
            title: qsTr("Please select an executable file")
            options: FileDialog.ReadOnly
            nameFilters: ["Executable files (*.exe)"]

            onAccepted: {
                AppData.settings.setExecutablePath(fileDialog.selectedFile)
                console.log("You selected: " + fileDialog.selectedFile)
            }
        }
    }

    Rectangle {
        id: fileRect

        Layout.columnSpan: 2
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignLeft

        radius: 10
        clip: true
        color: Theme.divider

        ToolTip {
            parent: parent
            visible: selectedFileLabel.text.length ? mouseArea.containsMouse : false
            text: selectedFileLabel.text
            font: Globals.font
            Material.background: Theme.divider
        }

        Label {
            id: selectedFileLabel

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: Globals.elementsLeftMargin
            anchors.rightMargin: Globals.elementsLeftMargin

            text: AppData.settings.executablePath
            font: Globals.font
            elide: Qt.ElideLeft
            horizontalAlignment: Text.AlignHCenter
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

        Material.background: Theme.primary

        text: qsTr("Start")
        font: Globals.font
        onClicked: AppData.serverProcess.start()
    }
    Rectangle {
        id: statusRect

        Layout.alignment: Qt.AlignRight
        Layout.preferredWidth: startButton.width * 1.2
        Layout.fillHeight: true

        radius: 10
        color: Theme.divider
        border.color: Theme.alert
        border.width: 2

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

        Material.background: Theme.alert

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
                enabled: false
            }
            PropertyChanges {
                target: statusLabel
                text: qsTr("Running...")
            }
            PropertyChanges {
                target: statusRect
                border.color: Theme.primary
            }
        },
        State {
            name: "stopped"
            when: !panel.isRunning
            PropertyChanges {
                target: stopButton
                enabled: false
            }
        }
    ]
}
