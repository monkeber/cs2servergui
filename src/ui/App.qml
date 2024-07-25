import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import application 1.0

ApplicationWindow {
    id: window

    width: Screen.width / 2
    height: Screen.height / 2

    title: "CS2 Server Manager"
    visible: true
    color: Globals.backgroundColor

    menuBar: AppMenuBar {}

    ScrollView {
        id: scrollArea
        anchors.fill: parent
        anchors.topMargin: mainContent.spacing

        // Define a custom scroll bar so we can access its width and make scroll bar not overlay with content.
        ScrollBar.vertical: ScrollBar {
            id: scrollBar
            parent: scrollArea
            anchors.top: scrollArea.top
            anchors.right: scrollArea.right
            height: scrollArea.availableHeight
            policy: ScrollBar.AlwaysOn
        }
        ScrollBar.horizontal: ScrollBar {
            parent: scrollArea
            anchors.top: scrollArea.top
            anchors.right: scrollArea.right
            height: scrollArea.availableHeight
            policy: ScrollBar.AsNeeded
        }

        ColumnLayout {
            id: mainContent
            // ColumnLayout here is a child of contentItem property of scrollArea, so we can't use parent.width here.
            width: scrollArea.width - scrollBar.width
            PowerPanel {
                Layout.fillWidth: true
            }
            // TODO: Find a way to redirect logs to terminal output.
            // Spoiler {
            //     width: parent.width
            //     TerminalOutput {
            //         id: termOut
            //         height: window.height / 3
            //     }
            // }
            MainMenu {
                Layout.fillWidth: true
            }
        }
    }
}
