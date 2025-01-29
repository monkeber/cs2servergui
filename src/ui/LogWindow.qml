import QtQuick 6.2
import QtQuick.Controls
import application 1.0

ApplicationWindow {
    id: rootWindow
    title: qsTr("Logs")

    width: Screen.width / 2
    height: Screen.height / 3

    ScrollView {
        id: view
        anchors.fill: parent

        TextArea {
            id: area

            readOnly: true
            background: null
            wrapMode: TextEdit.WordWrap
        }
    }

    Connections {
        target: AppData.log
        onNewLogMessageCaptured: msg => {
                                     area.append(msg)
                                 }
    }
}
