import QtQuick 6.2
import QtQuick.Controls
import application 1.0

ApplicationWindow {
    id: rootWindow
    title: qsTr("Logs")

    width: Screen.width / 2
    height: Screen.height / 3

    ListModel {
        id: listModel
    }
    ScrollView {
        anchors.fill: parent
        contentWidth: rootWindow.width * 2

        clip: true

        ScrollBar.horizontal: ScrollBar {
            active: true
            visible: true
        }

        ListView {
            id: view
            anchors.fill: parent
            leftMargin: Globals.elementsLeftMargin

            model: listModel
            delegate: Label {
                text: content
                font: Globals.font
            }

            ScrollBar.vertical: ScrollBar {
                active: true
                visible: true
            }
        }
    }

    Connections {
        target: AppData.log
        onNewLogMessageCaptured: msg => {
                                     listModel.append({
                                                          "content": msg
                                                      })
                                     view.positionViewAtEnd()
                                 }
    }
}
