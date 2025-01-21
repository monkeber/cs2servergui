import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts
import application 1.0

ColumnLayout {
    HorizontalHeaderView {
        id: horizontalHeader
        Layout.fillWidth: true
        syncView: view
        clip: true
    }
    TableView {
        id: view

        readonly property int rowHeight: Screen.height / 10
        readonly property int columnWidth: view.width / view.model.columnCount()

        Layout.fillHeight: true
        Layout.fillWidth: true

        clip: true
        leftMargin: Globals.elementsLeftMargin
        flickableDirection: Flickable.HorizontalAndVerticalFlick
        // Provide implicit height so the table view can be positioned by a layout.
        implicitHeight: rowHeight * rows

        model: AppData.mapHistory.model

        delegate: Loader {
            sourceComponent: column === 3 ? imageDelegate : textDelegate

            Component {
                id: textDelegate
                Item {
                    implicitWidth: view.columnWidth
                    implicitHeight: view.rowHeight
                    Label {
                        anchors.centerIn: parent
                        // anchors.fill: parent
                        text: display
                        wrapMode: Text.WordWrap
                    }
                }
            }
            Component {
                id: imageDelegate
                Item {
                    implicitWidth: view.columnWidth
                    implicitHeight: view.rowHeight
                    Image {
                        anchors.fill: parent
                        source: new URL(display)
                        fillMode: Image.PreserveAspectFit
                    }
                }
            }
        }
    }
}
