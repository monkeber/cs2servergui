import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import cs2server 1.0

Window {
    id: window
    visible: true
    title: "CS2 Server Manager"
    color: Constants.backgroundColor
    width: Screen.width / 2
    height: Screen.height / 2

    ScrollView {
        id: scrollArea
        anchors.fill: parent

        // Define a custom scroll bar so we can access its width and make scroll bar not overlay with content.
        ScrollBar.vertical: ScrollBar {
            id: scrollBar
            parent: scrollArea
            anchors.top: scrollArea.top
            anchors.right: scrollArea.right
            height: scrollArea.availableHeight
            policy: ScrollBar.AlwaysOn
        }

        Column {
            // Column here is a child of contentItem property of scrollArea, so we can't use parent.width here.
            width: scrollArea.width - scrollBar.width
            PowerPanel {
                width: parent.width
            }

            Spoiler {
                width: parent.width
                TerminalOutput {
                    id: termOut
                    height: window.height / 3
                }
            }
            MainMenu {
                width: parent.width
            }
            Rectangle {
                width: parent.width
                height: 100
            }
        }
    }
}
