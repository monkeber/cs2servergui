import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts
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

        delegate: GridLayout {
            rows: 1
            columns: 4

            width: view.width
            Label {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter

                text: workshopID
                font: Globals.font
            }
            Label {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter

                text: mapName
                font: Globals.font
            }
            Label {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter

                text: downloadedAt
                font: Globals.font
            }
            Image {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignRight

                source: previewPath
                fillMode: Image.PreserveAspectFit
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

    function appendElement(element) {
        listModel.append({
                             "workshopID": element.workshopID,
                             "mapName": element.mapName,
                             "downloadedAt": element.downloadedAt,
                             "previewPath": new URL(element.previewPath)
                         })
    }

    Component.onCompleted: {
        AppData.mapHistory.get().forEach(appendElement)
    }

    Connections {
        target: AppData.mapHistory
        onEntryAdded: {
            listModel.clear()
            AppData.mapHistory.get().forEach(appendElement)
        }
    }
}
