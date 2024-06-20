import QtQuick 6.2
import QtQuick.Layouts
import application 1.0

ColumnLayout {
    id: column
    InputField {
        text: "Server Start Parameters"
        Layout.fillWidth: true
        height: 20

        input.onEditingFinished: Settings.startParameters = input.text
        input.text: Settings.startParameters
    }
    InputField {
        text: "Server IP"
        Layout.fillWidth: true
        height: 20

        input.onEditingFinished: Settings.serverIP = input.text
        input.text: Settings.serverIP
    }
    InputField {
        text: "RCON Password"
        Layout.fillWidth: true
        height: 20

        input.onEditingFinished: Settings.rconPass = input.text
        input.text: Settings.rconPass
    }
    InputField {
        text: "RCON Port"
        Layout.fillWidth: true
        height: 20

        input.onEditingFinished: Settings.rconPort = parseInt(input.text)
        input.text: Settings.rconPort
        input.inputMethodHints: Qt.ImhDigitsOnly
        input.validator: IntValidator {
            bottom: 0
        }
    }
}
