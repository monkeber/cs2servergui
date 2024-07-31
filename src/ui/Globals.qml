pragma Singleton

import QtQuick 6.2
import application 1.0

QtObject {
    // Values for computing necessary sizes.
    readonly property int refDisplayWidth: 2560
    readonly property int refDisplayHeight: 1440
    readonly property real refPixelDensity: 4.286910744197176

    property real sizeRatio: 1
    property real sizeRatioFont: 1

    function size(n) {
        return n * sizeRatio * AppData.settings.scaleFactor
    }

    property int inputFieldsHeight: size(20)
    property int menuBarHeight: size(30)

    property string relativeFontDirectory: "fonts"
    /* Edit this comment to add your custom font */
    readonly property font font: Qt.font({
                                             "family": Qt.application.font.family,
                                             "pixelSize": Qt.application.font.pixelSize
                                                          * sizeRatioFont
                                                          * AppData.settings.scaleFactor
                                         })
    readonly property font largeFont: Qt.font({
                                                  "family": Qt.application.font.family,
                                                  "pixelSize": Qt.application.font.pixelSize * 1.6 * sizeRatioFont * AppData.settings.scaleFactor
                                              })

    readonly property color backgroundColor: "#EAEAEA"

    Component.onCompleted: {
        let height = Screen.height
        let width = Screen.width
        let dpi = Screen.pixelDensity

        sizeRatio = Math.min(height / refDisplayHeight, width / refDisplayWidth)
        sizeRatioFont = Math.min(
            height * refPixelDensity / (dpi * refDisplayHeight),
            width * refPixelDensity / (dpi * refDisplayWidth))
    }
}
