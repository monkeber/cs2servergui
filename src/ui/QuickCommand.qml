import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts
import application 1.0

RowLayout {
    id: main

    property alias text: input.text
    required property int index
    required property string initialText

    signal deleteRequested

    Button {
        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
        Layout.fillHeight: true

        font: Globals.font
        text: qsTr("Execute")

        Material.background: Theme.primary

        onClicked: {
            if (input.text.length > 0) {
                AppData.serverProcess.execCommand(input.text)
            }
        }
    }
    TextField {
        id: input

        Layout.fillWidth: true
        Layout.fillHeight: true

        font: Globals.font
        text: main.initialText

        onAccepted: {
            if (input.text.length > 0) {
                AppData.serverProcess.execCommand(input.text)
            }
        }
    }

    Button {
        id: deleteButton
        Layout.fillHeight: true

        font: Globals.font
        text: qsTr("Delete")

        Material.background: Theme.alert
    }

    Component.onCompleted: deleteButton.clicked.connect(deleteRequested)
}
