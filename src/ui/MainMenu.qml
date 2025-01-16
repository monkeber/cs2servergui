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
        TabButton {
            text: qsTr("Map History")
            font: Globals.font
        }
    }

    SwipeView {
        id: view
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.leftMargin: main.spacing

        currentIndex: bar.currentIndex
        interactive: false
        clip: true

        // TODO: the setting of the visibility was required because SwipeView would show the
        // adjacent pages when being resized. This is still not a proper fix because
        // it during resizing the page could wander off the window.
        ConsolePage {
            visible: view.currentIndex === 0
        }
        ServerParametersPage {
            visible: view.currentIndex === 1
        }
        MapHistoryPage {
            visible: view.currentIndex === 2
        }
    }
}
