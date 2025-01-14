import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts
import application 1.0

Item {
    TableView {
        id: view

        anchors.fill: parent
        leftMargin: Globals.elementsLeftMargin

        flickableDirection: Flickable.HorizontalAndVerticalFlick

        // We need to set content width to the maximum width of listview elements in order to have working horizontal scroll.
        // contentWidth: view.contentItem.childrenRect.width + ScrollBar.vertical.width
        model: AppData.mapHistory.model

        delegate: Rectangle {
            implicitWidth: 100
            implicitHeight: 50
            Text {
                text: display
            }
        }
    }
}
