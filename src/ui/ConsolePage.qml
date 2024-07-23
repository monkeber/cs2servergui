import QtQuick 6.2
import QtQuick.Layouts
import application 1.0

ColumnLayout {
    id: column
    InputField {
        Layout.fillWidth: true
        Layout.preferredHeight: Globals.inputFieldsHeight

        text: "Console"
        input.onAccepted: {
            if (input.text.length > 0) {
                AppData.serverProcess.execCommand(input.text)
                input.text = ""
            }
        }

        input.Keys.onPressed: event => {
                                  if (event.key === Qt.Key_Up) {
                                      console.log(
                                          AppData.serverProcess.getGeneralHistory(
                                              ))
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

            text: "Host Workshop Map"
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

            text: "Exec"
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
}
