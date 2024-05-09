import QtQuick 6.2
import cs2server 1.0

Column {
    id: column
    InputField {
        text: "Server Start Parameters"
        anchors.left: parent.left
        anchors.right: parent.right
        height: 20

        input.onTextEdited: ProcessHandler.setStartPatameters(text)
    }
}
