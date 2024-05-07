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
            text: qsTr("Discover")
        }
        TabButton {
            text: qsTr("Activity")
        }
    }

    StackLayout {
        anchors.right: parent.right
        anchors.left: parent.left
        currentIndex: bar.currentIndex
        ConsolePage {
            id: homeTab
        }
        Rectangle {
            id: discoverTab
            color: "blue"
            implicitHeight: 100
        }
        Rectangle {
            id: activityTab
            color: "yellow"
            implicitHeight: 200
        }
    }
}
