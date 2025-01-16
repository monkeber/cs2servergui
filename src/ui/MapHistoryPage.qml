import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts
import application 1.0

TableView {
    id: view

    clip: true
    leftMargin: Globals.elementsLeftMargin

    flickableDirection: Flickable.HorizontalAndVerticalFlick

    model: AppData.mapHistory.model

    delegate: Loader {
        sourceComponent: column === 3 ? imageDelegate : textDelegate

        Component {
            id: textDelegate
            Item {
                implicitWidth: view.width / view.model.columnCount()
                implicitHeight: 50
                Text {
                    text: display
                }
            }
        }
        Component {
            id: imageDelegate
            Item {
                implicitWidth: view.width / view.model.columnCount()
                implicitHeight: 50
                Image {
                    anchors.fill: parent
                    source: new URL(display)
                    fillMode: Image.PreserveAspectFit
                }
            }
        }
    }
}
