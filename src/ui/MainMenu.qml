import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    TabBar {
        id: bar
        Layout.fillWidth: true
        TabButton {
            text: qsTr("Console")
            font: Globals.font
        }
        TabButton {
            text: qsTr("Server Parameters")
            font: Globals.font
        }
    }

    SwipeView {
        Layout.fillWidth: true
        currentIndex: bar.currentIndex
        interactive: false
        clip: true
        ConsolePage {
            id: homeTab
        }
        ServerParametersPage {
            id: discoverTab
        }
    }
}
