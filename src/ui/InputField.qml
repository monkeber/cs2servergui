import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts
import application 1.0

RowLayout {
    property string text
    property alias input: input

    Label {
        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

        font: Globals.font
        text: parent.text + ":"
    }
    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true

        color: "transparent"
        border.color: "black"
        border.width: 2
        TextInput {
            id: input

            anchors.fill: parent
            anchors.leftMargin: parent.border.width * 2
            anchors.rightMargin: parent.border.width * 2

            focus: true
            selectByMouse: true
            clip: true
            font: Globals.font
        }

        HoverHandler {
            acceptedDevices: PointerDevice.Mouse
            cursorShape: Qt.IBeamCursor
        }
    }
}
