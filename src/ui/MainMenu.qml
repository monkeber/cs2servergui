import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: main
    TabBar {
        id: bar
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop
        TabButton {
            text: qsTr("Console")
            font: Globals.font
        }
        TabButton {
            text: qsTr("Server Parameters")
            font: Globals.font
        }
        TabButton {
            text: qsTr("Map History")
            font: Globals.font
        }
    }

    ConsolePage {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.leftMargin: Globals.elementsLeftMargin
        visible: bar.currentIndex === 0
    }
    ServerParametersPage {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.leftMargin: Globals.elementsLeftMargin
        Layout.rightMargin: Globals.elementsLeftMargin
        Layout.maximumHeight: implicitHeight
        visible: bar.currentIndex === 1
    }
    MapHistoryPage {
        Layout.fillWidth: true
        Layout.fillHeight: true
        visible: bar.currentIndex === 2
    }

    // A spacer to push other elements to top.
    Item {
        Layout.fillHeight: true
    }
}
