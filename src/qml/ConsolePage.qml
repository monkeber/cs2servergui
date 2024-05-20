import QtQuick 6.2
import cs2server 1.0

Column {
    id: column
    InputField {
        text: "Console"
        anchors.left: parent.left
        anchors.right: parent.right
        height: 20

        input.onEditingFinished: {
            if (input.text.length > 0) {
                ProcessHandler.execCommand(input.text)
                input.text = ""
            }
        }
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
