import QtQuick 6.2
import QtQuick.Controls
import Qt.labs.qmlmodels
import application 1.0

ApplicationWindow {
    id: rootWindow
    title: qsTr("Map History")

    width: Screen.width / 2
    height: Screen.height / 3

    ListModel {
        id: listModel
    }
    ListView {
        id: view

        anchors.fill: parent
        leftMargin: Globals.elementsLeftMargin

        flickableDirection: Flickable.HorizontalAndVerticalFlick
        // We need to set content width to the maximum width of listview elements in order to have working horizontal scroll.
        contentWidth: view.contentItem.childrenRect.width + ScrollBar.vertical.width

        model: listModel

        delegate: Row {
            Label {
                text: workshopID
                font: Globals.font
            }
            Label {
                text: mapName
                font: Globals.font
            }
            Label {
                text: downloadedAt
                font: Globals.font
            }
            Image {
                source: previewPath
                // width: 130
                // height: 100
                // fillMode: Image.PreserveAspectFit
            }
        }

        ScrollBar.vertical: ScrollBar {
            active: true
            visible: true
        }

        ScrollBar.horizontal: ScrollBar {
            active: true
            visible: true
        }
    }
    Component.onCompleted: {
        listModel.append({
                             "workshopID": "some",
                             "mapName": "some map",
                             "downloadedAt": "2024",
                             "previewPath": new URL("file:///E:/Projects/cs2guiserver/build/Debug-Desktop_Qt_6_8_1_MSVC2022_64bit/previews/preview_3379564935.jpeg")
                         })
    }
}
