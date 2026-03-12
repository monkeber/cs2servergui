import QtQuick 6
import QtQuick.Controls
import QtQuick.Layouts
import application 1.0

ColumnLayout {
    id: column

    GridLayout {
        id: layoutGrid
        columns: 2

        Layout.leftMargin: Globals.elementsLeftMargin
        Layout.rightMargin: Globals.elementsLeftMargin

        Repeater {
            model: [ qsTr("Console"), qsTr("Host Workshop Map"), qsTr("Exec") ]

            delegate: Label {
                required property string modelData
                required property int index

                font: Globals.font
                text: modelData + ":"

                Layout.column: 0
                Layout.row: index
            }
        }

        // Repeater's model doesn't want to use functions and they become undefined for some reason, so we use just an array property.
        property var actionsModel: [
            {
                pressHandler: function(event, input) {
                    if (event.key === Qt.Key_Up) {
                        input.text = AppData.serverProcess.getGeneralHistory(
                            ).getOlder()
                    }
                    if (event.key === Qt.Key_Down) {
                        input.text = AppData.serverProcess.getGeneralHistory(
                            ).getNewer()
                    }
                },
                acceptHandler: function(input) {
                    if (input.text.length > 0) {
                        AppData.serverProcess.execCommand(input.text)
                        input.text = ""
                    }
                }
            },
            {
                pressHandler: null,
                acceptHandler: function(input) {
                    if (input.text.length > 0) {
                        AppData.serverProcess.hostWorkshopMap(input.text)
                        input.text = ""
                    }
                }
            },
            {
                pressHandler: function(event, input) {
                    if (event.key === Qt.Key_Up) {
                        input.text = AppData.serverProcess.getScriptHistory(
                            ).getOlder()
                    }
                    if (event.key === Qt.Key_Down) {
                        input.text = AppData.serverProcess.getScriptHistory(
                            ).getNewer()
                    }
                },
                acceptHandler: function(input) {
                    if (input.text.length > 0) {
                        AppData.serverProcess.execScriptName(input.text)
                        input.text = ""
                    }
                }
            }
        ]

        Repeater {
            model: layoutGrid.actionsModel

            delegate: TextField {
                required property int index

                Layout.column: 1
                Layout.row: index

                Layout.fillWidth: true
                Layout.preferredHeight: Globals.inputFieldsHeight

                font: Globals.font

                onAccepted: layoutGrid.actionsModel[index].acceptHandler(this);
                Keys.onPressed: event => {
                    if (layoutGrid.actionsModel[index].pressHandler)
                        layoutGrid.actionsModel[index].pressHandler(event, this);
                }
            }
        }
    }

    Button {
        Layout.alignment: Qt.AlignRight
        text: qsTr("Reset RCON connection")
        font: Globals.font
        Material.background: Theme.neutral

        onClicked: AppData.serverProcess.resetRconConnection();
    }

    //
    // Quick commands section.
    //

    ListModel {
        id: model

        Component.onCompleted: {
            for (const cmd of AppData.settings.quickCommands) {
                column.createQuickCommand(cmd)
            }
        }
    }

    Connections {
        target: Qt.application
        function onAboutToQuit() {
            AppData.settings.quickCommands = []
            for (var i = 0; i < model.count; i++) {
                var cmdText = model.get(i).initialText
                if (cmdText.length > 0) {
                    AppData.settings.quickCommands.push(cmdText)
                }
            }
        }
    }

    function createQuickCommand(commandText = "") {
        model.append({
            "initialText": commandText
        })
    }

    Button {
        Layout.alignment: Qt.AlignLeft
        text: qsTr("Add quick command")
        font: Globals.font
        Material.background: Theme.primary

        onClicked: column.createQuickCommand()
    }

    ListView {
        id: listView

        Layout.fillHeight: true
        Layout.fillWidth: true

        clip: true
        spacing: 5

        WheelHandler {
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            onWheel: event => {
                var notches = event.angleDelta.y / 120
                listView.contentY = Math.max(0, Math.min(
                    listView.contentY - notches * Globals.inputFieldsHeight,
                    listView.contentHeight - listView.height
                ))
                event.accepted = true
            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: listView.contentHeight > listView.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
        }

        contentWidth: ScrollBar.vertical.visible ? listView.width - ScrollBar.vertical.width : listView.width

        model: model

        delegate: QuickCommand {
            required property string initialText
            required property int index

            onDeleteRequested: model.remove(index)
            width: listView.contentWidth
            height: Globals.inputFieldsHeight

            text: initialText

            onTextChanged: model.setProperty(index, "initialText", text)
        }
    }
}
