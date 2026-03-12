import QtQuick 6
import QtQuick.Controls
import QtQuick.Layouts
import application 1.0

ColumnLayout {
    Rectangle {
        Layout.fillWidth: true

        height: 1

        color: Theme.divider
    }

    //
    // Filters rows.
    //
    RowLayout {
        Layout.leftMargin: Globals.elementsLeftMargin
        Layout.rightMargin: Globals.elementsLeftMargin

        ButtonGroup {
            id: buttonGroup
            exclusive: false

            onClicked: {
                AppData.mapHistory.model.UpdateFilters(
                    buttons.find(button => button.objectName == "sortByRating").checked,
                    buttons.find(button => button.objectName == "removeDuplicates").checked,
                    buttons.find(button => button.objectName == "showOnlyBookmarks").checked
                );
            }
        }

        Button {
            icon.source: "qrc:///images/filter.svg"
            icon.color: highlighted ? "white" : Theme.accent

            highlighted: buttonGroup.checkState !== Qt.Unchecked
            Material.background: highlighted ? Theme.accent : Qt.lighter(Theme.divider, 1.3)

            onClicked: {
                buttonGroup.checkState = Qt.Unchecked;
                AppData.mapHistory.model.UpdateFilters(false, false, false);
            }
        }

        ListModel {
            id: buttonsModel
            ListElement { text: qsTr("Sort by rating"); name: "sortByRating" }
            ListElement { text: qsTr("Remove duplicates"); name: "removeDuplicates" }
            ListElement { text: qsTr("Show only bookmarked"); name: "showOnlyBookmarks" }
        }

        Repeater {
            model: buttonsModel

            delegate: Button {
                Layout.fillWidth: true

                checkable: true
                text: model.text
                font: Globals.font
                objectName: model.name

                highlighted: checked
                Material.background: checked ? Theme.accent : Qt.lighter(Theme.divider, 1.3)
                ButtonGroup.group: buttonGroup
            }
        }
    }

    Rectangle {
        Layout.fillWidth: true

        height: 1

        color: Theme.divider
    }

    //
    // Table header.
    //
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

    //
    // Table.
    //
    TableView {
        id: view

        readonly property int rowHeight: Screen.height * AppData.settings.scaleFactor / 10

        WheelHandler {
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            onWheel: event => {
                var notches = event.angleDelta.y / 120
                view.contentY = Math.max(0, Math.min(
                    view.contentY - notches * Globals.inputFieldsHeight,
                    view.contentHeight - view.height
                ))
                event.accepted = true
            }
        }

        columnWidthProvider: function (column) {
            // Column width should account for margins, otherwise margins don't work.
            // Divide view width into parts and make most of the columns take 2 parts, while 2 columns will take only 1 part.
            const bookmarkWidth = 50;
            const partWidth = (view.width - leftMargin - rightMargin - bookmarkWidth) / (view.model.columnCount() - 1);
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
        reuseItems: false

        ScrollBar.vertical: ScrollBar {
            id: scrollBar
            policy: ScrollBar.AlwaysOn
        }

        model: AppData.mapHistory.model

        delegate: Loader {
            id: rootLoader
            property var displayData: display

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

                        text: rootLoader.displayData
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

                        source: rootLoader.displayData.length
                                === 0 ? "qrc:///images/no_preview.png" : new URL(rootLoader.displayData)
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

                        onClicked: view.model.removeRow(row)
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

                        text: rootLoader.displayData
                        font: Globals.font
                        color: Theme.foreground
                    }
                }
            }
            Component {
                id: bookmarkDelegate

                Item {
                    implicitHeight: view.rowHeight
                    CheckBox {
                        checked: rootLoader.displayData
                        checkable: true

                        // Set the size explicitly, otherwise Qt detects a binding loop if we fill parent anchors.
                        height: parent.height
                        width: parent.width

                        indicator: Image {
                            anchors.fill: parent
                            source: parent.checked ? "qrc:///images/bookmark_checked.svg" : "qrc:///images/bookmark_unchecked.svg"
                            fillMode: Image.PreserveAspectFit
                        }

                        onToggled: AppData.mapHistory.model.UpdateBookmarked(row, checked);
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
                        currentIndex: rootLoader.displayData

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
