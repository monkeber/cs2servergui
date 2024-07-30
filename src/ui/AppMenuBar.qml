import QtQuick
import QtQuick.Controls
import application 1.0

MenuBar {
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

    delegate: MenuBarItem {
        font: Globals.font
        height: parent.height
    }
}
