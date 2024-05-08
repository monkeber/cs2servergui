import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts

Column {
    TabBar {
        id: bar
        anchors.right: parent.right
        anchors.left: parent.left
        TabButton {
            text: qsTr("Console")
        }
        TabButton {
            text: qsTr("Server Parameters")
        }
    }

    SwipeView {
        anchors.right: parent.right
        anchors.left: parent.left
        currentIndex: bar.currentIndex
        clip: true
        ConsolePage {
            id: homeTab
        }
        ServerParametersPage {
            id: discoverTab
        }
    }
}
