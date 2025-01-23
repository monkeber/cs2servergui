import QtQuick 6.2
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

        InputField {
            Layout.fillWidth: true
            Layout.preferredHeight: Globals.inputFieldsHeight

            text: qsTr("Console")
            input.onAccepted: {
                if (input.text.length > 0) {
                    AppData.serverProcess.execCommand(input.text)
                    input.text = ""
                }
            }

            input.Keys.onPressed: event => {
                                      if (event.key === Qt.Key_Up) {
                                          input.text = AppData.serverProcess.getGeneralHistory(
                                              ).getOlder()
                                      }
                                      if (event.key === Qt.Key_Down) {
                                          input.text = AppData.serverProcess.getGeneralHistory(
                                              ).getNewer()
                                      }
                                  }
        }
        RowLayout {
            Layout.preferredHeight: Globals.inputFieldsHeight
            Layout.fillWidth: true

            // Set both inputs fields preferredWidth to the same value so they will occupy exactly half of the row.
            InputField {
                Layout.fillHeight: true
                Layout.preferredWidth: 1

                text: qsTr("Host Workshop Map")
                input.onAccepted: {
                    if (input.text.length > 0) {
                        AppData.serverProcess.hostWorkshopMap(input.text)
                        input.text = ""
                    }
                }
            }
            InputField {
                Layout.fillHeight: true
                Layout.preferredWidth: 1

                text: qsTr("Exec")
                input.onAccepted: {
                    if (input.text.length > 0) {
                        AppData.serverProcess.execScriptName(input.text)
                        input.text = ""
                    }
                }
                input.Keys.onPressed: event => {
                                          if (event.key === Qt.Key_Up) {
                                              input.text = AppData.serverProcess.getScriptHistory(
                                                  ).getOlder()
                                          }
                                          if (event.key === Qt.Key_Down) {
                                              input.text = AppData.serverProcess.getScriptHistory(
                                                  ).getNewer()
                                          }
                                      }
            }
        }

        Button {
            Layout.alignment: Qt.AlignLeft
            text: qsTr("Add quick command")
            font: Globals.font
            onClicked: column.createQuickCommand()

            Material.background: Theme.primary
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
