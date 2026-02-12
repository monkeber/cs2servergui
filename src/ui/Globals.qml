pragma Singleton

import QtQuick 6.2
import application 1.0

QtObject {
    // Values for computing necessary sizes.
    property real sizeRatio: 1
    property real sizeRatioFont: 1

    function size(n) {
        return n * sizeRatio * AppData.settings.scaleFactor
    }

    property int inputFieldsHeight: size(35)
    property int menuBarHeight: size(30)
    readonly property int elementsLeftMargin: size(5)
    readonly property int mapHistoryRowSpacing: size(5)

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
}
