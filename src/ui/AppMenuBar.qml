import QtQuick
import QtQuick.Controls
import application 1.0

MenuBar {
    clip: true
    Menu {
        id: fileMenu
        title: qsTr("View")
        font: Globals.font

        Action {
            text: qsTr("Zoom In (%1)".arg(shortcut))
            shortcut: "Ctrl+="
            onTriggered: {
                AppData.settings.scaleFactor += 0.05
            }
        }

        Action {
            text: qsTr("Zoom Out (%1)".arg(shortcut))
            shortcut: "Ctrl+-"
            onTriggered: {
                AppData.settings.scaleFactor -= 0.05
            }
        }

        Action {
            text: qsTr("Reset")
            onTriggered: {
                AppData.settings.scaleFactor = 1.0
            }
        }
    }

    delegate: MenuBarItem {
        font: Globals.font
        height: parent.height
    }
}
