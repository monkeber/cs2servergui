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

        columnWidthProvider: function (column) {
            // Column width should account for margins, otherwise margins don't work.
            // Divide view width into parts and make most of the columns take 2 parts, while 2 columns will take only 1 part.
            const bookmarkWidth = 50;
            const partWidth = (view.width - leftMargin - rightMargin - bookmarkWidth) / (view.model.columnCount() - 1)
            switch (column) {
                case MapHistoryModel.Columns.Bookmarked:
                    return bookmarkWidth;
                default:
                    return partWidth;
            }
        }

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
            sourceComponent: {
                switch (column) {
                case MapHistoryModel.Columns.MapWorkshopId:
                    return mapIdDelegate
                case MapHistoryModel.Columns.Rating:
                    return ratingDelegate
                case MapHistoryModel.Columns.Bookmarked:
                    return bookmarkDelegate
                case MapHistoryModel.Columns.Preview:
                    return imageDelegate
                default:
                    return textDelegate
                }
            }

            Component {
                id: textDelegate
                Item {
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
                }
            }
            Component {
                id: imageDelegate
                Item {
                    implicitHeight: view.rowHeight
                    Image {
                        anchors.fill: parent
                        anchors.topMargin: Globals.mapHistoryRowSpacing
                        anchors.bottomMargin: anchors.topMargin

                        source: display.length
                                === 0 ? "qrc:///images/no_preview.png" : new URL(display)
                        fillMode: Image.PreserveAspectFit
                    }
                }
            }
            Component {
                id: mapIdDelegate

                Item {
                    implicitHeight: view.rowHeight
                    Button {
                        id: delButton
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter

                        icon.source: "qrc:///images/delete_icon.svg"

                        Material.background: Theme.alert

                        onClicked: {
                            view.model.removeRow(row)
                        }
                    }
                    TextInput {
                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.left: delButton.right

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.WordWrap
                        selectByMouse: true
                        readOnly: true

                        text: display
                        font: Globals.font
                        color: Theme.foreground
                    }
                }
            }
            Component {
                id: bookmarkDelegate
                Item {
                    implicitHeight: view.rowHeight
                    Image {
                        anchors.fill: parent
                        anchors.topMargin: Globals.mapHistoryRowSpacing
                        anchors.bottomMargin: anchors.topMargin

                        source: display ? "qrc:///images/bookmark_checked.svg" : "qrc:///images/bookmark_unchecked.svg"
                        fillMode: Image.PreserveAspectFit
                    }
                }
            }
            Component {
                id: ratingDelegate
                Item {
                    ComboBox {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.right: parent.right
                        model: ["0", "1 ⭐", "2 ⭐⭐", "3 ⭐⭐⭐", "4 ⭐⭐⭐⭐", "5 ⭐⭐⭐⭐⭐"]
                        currentIndex: display

                        onActivated: AppData.mapHistory.model.UpdateRating(row, currentIndex);
                    }
                }
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
