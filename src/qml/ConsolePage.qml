import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts

Column {
    id: column
    InputField {
        text: "Console"
        anchors.left: parent.left
        anchors.right: parent.right
        height: 20
    }
    Row {
        height: 20
        anchors.left: parent.left
        anchors.right: parent.right
        InputField {
            text: "Host Workshop Map"
            height: parent.height
            width: parent.width / 2
        }
        InputField {
            text: "Exec"
            height: parent.height
            width: parent.width / 2
        }
    }
}
