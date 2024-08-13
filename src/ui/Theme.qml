pragma Singleton

import QtQuick 6.2
import QtQuick.Controls
import QtQml
import application 1.0

Item {
    id: theme

    enum Type {
        Light = 0,
        Dark = 1
    }
    function type2Str(type) {
        switch (type) {
        case Theme.Type.Dark:
            return "Dark"
        case Theme.Type.Light:
        default:
            return "Light"
        }
    }
    function str2Type(str) {
        switch (str) {
        case "Dark":
            return Theme.Type.Dark
        case "Light":
        default:
            return Theme.Type.Light
        }
    }

    property int currentTheme: Theme.Type.Light
    property int themeId: Material.Light
    property color background: Material.background
    property color foreground: Material.foreground
    property color primary: "#A5D6A7"
    property color accent: "#4caf50"
    property color divider: "#BDBDBD"
    property color alert: "#E57373"

    states: [
        State {
            name: "light"
            when: currentTheme === Theme.Type.Light
            PropertyChanges {
                target: theme
                themeId: Material.Light
                background: "#ededed"
                foreground: "#212121"
                primary: "#A5D6A7"
                divider: "#BDBDBD"
            }
        },
        State {
            name: "dark"
            when: currentTheme === Theme.Type.Dark
            PropertyChanges {
                target: theme
                themeId: Material.Dark
                background: "#212121"
                foreground: "#FFFFFF"
                primary: "#388E3C"
                divider: "#474747"
            }
        }
    ]

    onCurrentThemeChanged: AppData.settings.theme = type2Str(currentTheme)
    Component.onCompleted: currentTheme = str2Type(AppData.settings.theme)
}
