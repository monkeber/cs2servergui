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

    property int currentTheme: Theme.Type.Light
    property int themeId: Material.Light

    states: [
        State {
            name: "light"
            when: currentTheme === Theme.Type.Light
            PropertyChanges {
                target: theme
                themeId: Material.Light
            }
        },
        State {
            name: "dark"
            when: currentTheme === Theme.Type.Dark
            PropertyChanges {
                target: theme
                themeId: Material.Dark
            }
        }
    ]

    onCurrentThemeChanged: AppData.settings.theme = type2Str(currentTheme)
}
