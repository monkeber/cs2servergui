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
        interactive: false

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

        readonly property int rowHeight: Screen.height * AppData.settings.scaleFactor / 10
        // Column width should account for margins, otherwise margins don't work.
        readonly property int columnWidth: (view.width - leftMargin - rightMargin)
                                           / view.model.columnCount()

        Layout.fillHeight: true
        Layout.fillWidth: true

        // Provide implicit height so the table view can be positioned by a layout.
        implicitHeight: rowHeight * rows
        leftMargin: Globals.elementsLeftMargin
        rightMargin: scrollBar.width
        clip: true
        interactive: true
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.OvershootBounds

        ScrollBar.vertical: ScrollBar {
            id: scrollBar
            policy: ScrollBar.AlwaysOn
        }

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
                        color: Theme.foreground
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
