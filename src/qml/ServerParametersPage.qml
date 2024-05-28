import QtQuick 6.2
import cs2server 1.0

Column {
    id: column
    InputField {
        text: "Server Start Parameters"
        anchors.left: parent.left
        anchors.right: parent.right
        height: 20

        input.onTextEdited: Settings.startParameters = input.text
        input.text: Settings.startParameters
    }
    InputField {
        text: "Server IP"
        anchors.left: parent.left
        anchors.right: parent.right
        height: 20

        input.onEditingFinished: Settings.serverIP = input.text
        input.text: Settings.serverIP
    }
    InputField {
        text: "RCON Password"
        anchors.left: parent.left
        anchors.right: parent.right
        height: 20

        input.onEditingFinished: Settings.rconPass = input.text
        input.text: Settings.rconPass
    }
}
