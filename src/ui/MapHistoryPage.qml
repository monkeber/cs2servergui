import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts
import application 1.0

ColumnLayout {
    Rectangle {
        Layout.fillWidth: true

        height: 1

        color: Theme.divider
    }
    HorizontalHeaderView {
        id: horizontalHeader

        Layout.fillWidth: true

        syncView: view
        clip: true

        delegate: Item {
            implicitWidth: label.implicitWidth * 2
            implicitHeight: label.implicitHeight * 2
            Label {
                id: label
                anchors.centerIn: parent

                wrapMode: Text.WordWrap

                text: display
                font: Globals.font
            }
        }
    }
    Rectangle {
        Layout.fillWidth: true

        height: 1

        color: Theme.divider
    }
    TableView {
        id: view

        readonly property int rowHeight: Screen.height / 10
        readonly property int columnWidth: view.width / view.model.columnCount()

        Layout.fillHeight: true
        Layout.fillWidth: true

        // Provide implicit height so the table view can be positioned by a layout.
        implicitHeight: rowHeight * rows
        leftMargin: Globals.elementsLeftMargin
        clip: true
        interactive: false

        model: AppData.mapHistory.model

        delegate: Loader {
            sourceComponent: column === 3 ? imageDelegate : textDelegate

            Component {
                id: textDelegate
                Item {
                    implicitWidth: view.columnWidth
                    implicitHeight: view.rowHeight
                    TextInput {
                        anchors.fill: parent

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.WordWrap
                        selectByMouse: true
                        readOnly: true

                        text: display
                        font: Globals.font
                    }
                    Rectangle {
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right

                        height: 1

                        color: Theme.divider
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
                        anchors.topMargin: Globals.mapHistoryRowSpacing
                        anchors.bottomMargin: anchors.topMargin

                        source: new URL(display)
                        fillMode: Image.PreserveAspectFit
                    }
                    Rectangle {
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right

                        height: 1

                        color: Theme.divider
                    }
                }
            }
        }
    }
}
