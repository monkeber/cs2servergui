import QtQuick 6.2
import QtQuick.Controls
import application 1.0

MenuBar {
    Menu {
        id: fileMenu
        title: qsTr("Options")

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
    }
}
