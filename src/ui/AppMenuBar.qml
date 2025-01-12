import QtQuick
import QtQuick.Controls
import application 1.0

MenuBar {
    id: bar

    clip: true
    Menu {
        id: viewMenu
        title: qsTr("View")
        font: Globals.font

        Action {
            text: qsTr("Zoom In (%1)".arg(shortcut))
            shortcut: "Ctrl+="
            onTriggered: AppData.settings.scaleFactor += 0.05
        }

        Action {
            text: qsTr("Zoom Out (%1)".arg(shortcut))
            shortcut: "Ctrl+-"
            onTriggered: AppData.settings.scaleFactor -= 0.05
        }

        Action {
            text: qsTr("Reset")
            onTriggered: AppData.settings.scaleFactor = 1.0
        }

        Component.onCompleted: {
            let maxWidth = 0
            // Since menu appears to hold only menu items in that list we can use for each.
            // If the behaviour of Qt changes here we might encounter some visual bugs.
            viewMenu.contentData.forEach(function (child) {
                maxWidth = Math.max(maxWidth, child.width)
            })
            contentWidth = Qt.binding(function () {
                return maxWidth * AppData.settings.scaleFactor
            })
        }
    }

    Menu {
        id: themeMenu
        title: qsTr("Theme")
        font: Globals.font

        Action {
            text: qsTr("Light")
            onTriggered: Theme.currentTheme = Theme.Type.Light
        }

        Action {
            text: qsTr("Dark")
            onTriggered: Theme.currentTheme = Theme.Type.Dark
        }

        Component.onCompleted: {
            let maxWidth = 0
            // Since menu appears to hold only menu items in that list we can use for each.
            // If the behaviour of Qt changes here we might encounter some visual bugs.
            themeMenu.contentData.forEach(function (child) {
                maxWidth = Math.max(maxWidth, child.width)
            })
            contentWidth = Qt.binding(function () {
                return maxWidth * AppData.settings.scaleFactor
            })
        }
    }

    Menu {
        id: otherMenu
        title: qsTr("Other")
        font: Globals.font

        Action {
            text: qsTr("Map History")
            onTriggered: mapHistoryWindow.setVisible(!mapHistoryWindow.visible)
        }

        Component.onCompleted: {
            let maxWidth = 0
            // Since menu appears to hold only menu items in that list we can use for each.
            // If the behaviour of Qt changes here we might encounter some visual bugs.
            themeMenu.contentData.forEach(function (child) {
                maxWidth = Math.max(maxWidth, child.width)
            })
            contentWidth = Qt.binding(function () {
                return maxWidth * AppData.settings.scaleFactor
            })
        }
    }

    Menu {
        id: helpMenu
        title: qsTr("Help")
        font: Globals.font

        Action {
            text: qsTr("Show Logs")
            onTriggered: logWindow.setVisible(!logWindow.visible)
        }

        Component.onCompleted: {
            let maxWidth = 0
            // Since menu appears to hold only menu items in that list we can use for each.
            // If the behaviour of Qt changes here we might encounter some visual bugs.
            themeMenu.contentData.forEach(function (child) {
                maxWidth = Math.max(maxWidth, child.width)
            })
            contentWidth = Qt.binding(function () {
                return maxWidth * AppData.settings.scaleFactor
            })
        }
    }

    LogWindow {
        id: logWindow
        visible: false
    }

    MapHistoryWindow {
        id: mapHistoryWindow
        visible: false
    }

    delegate: MenuBarItem {
        font: Globals.font
        height: bar.height
    }

    background: Rectangle {
        color: Theme.divider
        Rectangle {
            anchors.fill: parent
            anchors.bottomMargin: 1

            color: bar.Material.background
        }
    }
}
