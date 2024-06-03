import QtQuick 6.2
import QtQuick.Layouts
import cs2server 1.0

ColumnLayout {
    id: column
    InputField {
        text: "Console"
        Layout.fillWidth: true
        height: 20

        input.onAccepted: {
            if (input.text.length > 0) {
                ProcessHandler.execCommand(input.text)
                input.text = ""
            }
        }

        input.Keys.onPressed: event => {
                                  if (event.key === Qt.Key_Up) {
                                      console.log(
                                          ProcessHandler.getGeneralHistory())
                                      input.text = ProcessHandler.getGeneralHistory(
                                          ).getOlder()
                                  }
                                  if (event.key === Qt.Key_Down) {
                                      input.text = ProcessHandler.getGeneralHistory(
                                          ).getNewer()
                                  }
                              }
    }
    Row {
        height: 20
        Layout.fillWidth: true
        InputField {
            text: "Host Workshop Map"
            height: parent.height
            width: parent.width / 2
            input.onAccepted: {
                if (input.text.length > 0) {
                    ProcessHandler.hostWorkshopMap(input.text)
                    input.text = ""
                }
            }
        }
        InputField {
            text: "Exec"
            height: parent.height
            width: parent.width / 2
            input.onAccepted: {
                if (input.text.length > 0) {
                    ProcessHandler.execScriptName(input.text)
                    input.text = ""
                }
            }
            input.Keys.onPressed: event => {
                                      if (event.key === Qt.Key_Up) {
                                          input.text = ProcessHandler.getScriptHistory(
                                              ).getOlder()
                                      }
                                      if (event.key === Qt.Key_Down) {
                                          input.text = ProcessHandler.getScriptHistory(
                                              ).getNewer()
                                      }
                                  }
        }
    }
}
