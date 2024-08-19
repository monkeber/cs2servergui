import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: main
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
        Layout.leftMargin: main.spacing

        currentIndex: bar.currentIndex
        interactive: false
        clip: true

        ConsolePage {}
        ServerParametersPage {}
    }
}
