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

    Material.theme: Theme.currentTheme
    Material.background: Theme.background
    Material.foreground: Theme.foreground
    Material.primary: Theme.primary
    Material.accent: Theme.accent

    menuBar: AppMenuBar {
        height: Globals.menuBarHeight
    }

    ColumnLayout {
        id: mainContent
        anchors.fill: parent
        anchors.topMargin: Globals.elementsLeftMargin

        PowerPanel {
            Layout.fillWidth: true
            Layout.leftMargin: Globals.elementsLeftMargin
            Layout.rightMargin: Globals.elementsLeftMargin
            Layout.maximumHeight: implicitHeight
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
            Layout.fillHeight: true
        }
    }
}
