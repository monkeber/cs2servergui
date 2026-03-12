import QtQuick 6
import QtQuick.Controls
import QtQuick.Layouts
import application 1.0

GridLayout {
    id: root
    columns: 2

    Repeater {
        model: [ qsTr("Server Start Parameters"), qsTr("Server IP"), qsTr("RCON Password"), qsTr("RCON Port") ]

        delegate: Label {
            required property string modelData
            required property int index

            font: Globals.font
            text: modelData + ":"

            Layout.column: 0
            Layout.row: index
        }
    }

    IntValidator {
        id: portValidator
        bottom: 0
        top: 65535
    }

    property var inputModel: [
        {
            finishedEditing: function(input) {
                AppData.settings.startParameters = input.text;
            },
            placeholder: AppData.settings.startParameters,
            inputHints: Qt.ImhNone,
            validator: null
        },
        {
            finishedEditing: function(input) {
                AppData.settings.serverIP = input.text;
            },
            placeholder: AppData.settings.serverIP,
            inputHints: Qt.ImhNone,
            validator: null
        },
        {
            finishedEditing: function(input) {
                AppData.settings.rconPass = input.text;
            },
            placeholder: AppData.settings.rconPass,
            inputHints: Qt.ImhNone,
            validator: null
        },
        {
            finishedEditing: function(input) {
                AppData.settings.rconPort = parseInt(input.text);
            },
            placeholder: AppData.settings.rconPort,
            inputHints: Qt.ImhDigitsOnly,
            validator: portValidator
        }
    ]

    Repeater {
        model: root.inputModel

        delegate: TextField {
            required property int index

            Layout.column: 1
            Layout.row: index

            Layout.fillWidth: true
            Layout.preferredHeight: Globals.inputFieldsHeight

            font: Globals.font

            onEditingFinished: root.inputModel[index].finishedEditing(this);
            text: root.inputModel[index].placeholder;
            inputMethodHints: root.inputModel[index].inputHints;
            validator: root.inputModel[index].validator;
        }
    }
}
