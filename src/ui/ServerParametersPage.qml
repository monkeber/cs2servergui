import QtQuick 6.2
import QtQuick.Layouts
import application 1.0

ColumnLayout {
    id: column
    InputField {
        text: qsTr("Server Start Parameters")
        Layout.fillWidth: true
        height: Globals.inputFieldsHeight

        input.onEditingFinished: AppData.settings.startParameters = input.text
        input.text: AppData.settings.startParameters
    }
    InputField {
        text: qsTr("Server IP")
        Layout.fillWidth: true
        height: Globals.inputFieldsHeight

        input.onEditingFinished: AppData.settings.serverIP = input.text
        input.text: AppData.settings.serverIP
    }
    InputField {
        text: qsTr("RCON Password")
        Layout.fillWidth: true
        height: Globals.inputFieldsHeight

        input.onEditingFinished: AppData.settings.rconPass = input.text
        input.text: AppData.settings.rconPass
    }
    InputField {
        text: qsTr("RCON Port")
        Layout.fillWidth: true
        height: Globals.inputFieldsHeight

        input.onEditingFinished: {
            AppData.settings.rconPort = parseInt(input.text)
        }
        input.text: AppData.settings.rconPort
        input.inputMethodHints: Qt.ImhDigitsOnly
        input.validator: IntValidator {
            bottom: 0
        }
    }
}
