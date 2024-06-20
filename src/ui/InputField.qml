import QtQuick 6.2
import QtQuick.Controls
import application 1.0

Row {
    property string text
    // property CommandHistory history
    property alias input: input

    Label {
        id: name
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        text: parent.text + ":"
    }
    Rectangle {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.width - name.width

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
        }

        HoverHandler {
            id: stylus
            acceptedDevices: PointerDevice.Mouse
            cursorShape: Qt.IBeamCursor
        }
    }
}
