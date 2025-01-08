import QtQuick 6.2
import QtQuick.Controls
import application 1.0

ApplicationWindow {

    // id: rootWindow
    // title: qsTr("Logs")

    // width: Screen.width / 2
    // height: Screen.height / 3

    // ListModel {
    //     id: listModel
    // }
    // ListView {
    //     id: view

    //     anchors.fill: parent
    //     leftMargin: Globals.elementsLeftMargin

    //     flickableDirection: Flickable.HorizontalAndVerticalFlick
    //     // We need to set content width to the maximum width of listview elements in order to have working horizontal scroll.
    //     contentWidth: view.contentItem.childrenRect.width + ScrollBar.vertical.width

    //     model: listModel
    //     delegate: Label {
    //         text: content
    //         font: Globals.font
    //     }

    //     ScrollBar.vertical: ScrollBar {
    //         active: true
    //         visible: true
    //     }

    //     ScrollBar.horizontal: ScrollBar {
    //         active: true
    //         visible: true
    //     }
    // }

    // Connections {
    //     target: AppData.log
    //     onNewLogMessageCaptured: msg => {
    //                                  listModel.append({
    //                                                       "content": msg
    //                                                   })
    //                                  view.positionViewAtEnd()
    //                              }
    // }
    Component.onCompleted: {

    }
}
