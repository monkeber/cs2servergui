import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts
import application 1.0

TableView {
    id: view

    QtObject {
        id: internal

        readonly property int rowHeight: Screen.height / 10
        readonly property int columnWidth: view.width / view.model.columnCount()
    }

    clip: true
    leftMargin: Globals.elementsLeftMargin
    flickableDirection: Flickable.HorizontalAndVerticalFlick
    // Provide implicit height so the table view can be positioned by a layout.
    implicitHeight: internal.rowHeight * rows

    model: AppData.mapHistory.model

    delegate: Loader {
        sourceComponent: column === 3 ? imageDelegate : textDelegate

        Component {
            id: textDelegate
            Item {
                implicitWidth: internal.columnWidth
                implicitHeight: internal.rowHeight
                Text {
                    text: display
                }
            }
        }
        Component {
            id: imageDelegate
            Item {
                implicitWidth: internal.columnWidth
                implicitHeight: internal.rowHeight
                Image {
                    anchors.fill: parent
                    source: new URL(display)
                    fillMode: Image.PreserveAspectFit
                }
            }
        }
    }
    // delegate: Rectangle {
    //     implicitWidth: view.width / view.model.columnCount()
    //     implicitHeight: internal.rowHeight
    //     color: Qt.rgba(Math.random(), Math.random(), Math.random(), 1)
    // }
}
