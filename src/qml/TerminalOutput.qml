import QtQuick 6.2
import QtQuick.Controls
import cs2server 1.0

Rectangle {
    border.color: "black"
    border.width: 5

    ListModel {
        id: listModel
    }
    ListView {
        id: view
        anchors.fill: parent

        model: listModel
        delegate: Text {
            text: content
        }
        ScrollBar.vertical: ScrollBar {
            active: true
        }
    }
    Connections {
        target: ProcessHandler
        onOutputChanged: {
            listModel.append({
                                 "content": ProcessHandler.output
                             })
            view.positionViewAtEnd()
        }
    }
}
