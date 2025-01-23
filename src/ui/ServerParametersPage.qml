import QtQuick 6.2
import QtQuick.Layouts
import application 1.0

ColumnLayout {
    id: column
    implicitHeight: startParams.implicitHeight + ipParams.implicitHeight
                    + rconPassParams.implicitHeight + rconPortParams.implicitHeight

    InputField {
        id: startParams
        text: qsTr("Server Start Parameters")
        Layout.fillWidth: true
        Layout.preferredHeight: Globals.inputFieldsHeight

        input.onEditingFinished: AppData.settings.startParameters = input.text
        input.text: AppData.settings.startParameters
    }
    InputField {
        id: ipParams
        text: qsTr("Server IP")
        Layout.fillWidth: true
        Layout.preferredHeight: Globals.inputFieldsHeight

        input.onEditingFinished: AppData.settings.serverIP = input.text
        input.text: AppData.settings.serverIP
    }
    InputField {
        id: rconPassParams
        text: qsTr("RCON Password")
        Layout.fillWidth: true
        Layout.preferredHeight: Globals.inputFieldsHeight

        input.onEditingFinished: AppData.settings.rconPass = input.text
        input.text: AppData.settings.rconPass
    }
    InputField {
        id: rconPortParams
        text: qsTr("RCON Port")
        Layout.fillWidth: true
        Layout.preferredHeight: Globals.inputFieldsHeight

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
