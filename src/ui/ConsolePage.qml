import QtQuick 6
import QtQuick.Controls
import QtQuick.Layouts
import application 1.0

ScrollView {
    id: scrollArea

    ScrollBar.vertical: ScrollBar {
        id: scrollBar
        parent: scrollArea
        anchors.top: scrollArea.top
        anchors.right: scrollArea.right
        height: scrollArea.availableHeight
        policy: ScrollBar.AlwaysOn
    }

    ColumnLayout {
        id: column
        width: scrollArea.width - scrollBar.width

        GridLayout {
            id: layoutGrid
            columns: 2

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

        Button {
            Layout.alignment: Qt.AlignLeft
            text: qsTr("Add quick command")
            font: Globals.font
            Material.background: Theme.primary

            onClicked: column.createQuickCommand()
        }

        ListModel {
            id: model
        }

        Repeater {
            id: repeater

            delegate: QuickCommand {
                onDeleteRequested: model.remove(index)
            }
            model: model

            onItemAdded: (_, item) => item.Layout.preferredHeight = Qt.binding(
                function () {
                    return Globals.inputFieldsHeight
                })
        }
        function createQuickCommand(commandText = "") {
            model.append({
                "initialText": commandText
            })
        }

        Component.onCompleted: {
            for (const cmd of AppData.settings.quickCommands) {
                createQuickCommand(cmd)
            }
        }

        Component.onDestruction: {
            AppData.settings.quickCommands = []
            for (var i = 0; i < repeater.count; i++) {
                let cmdText = repeater.itemAt(i).text
                if (cmdText.length > 0) {
                    AppData.settings.quickCommands.push(cmdText)
                }
            }
        }
    }
}
