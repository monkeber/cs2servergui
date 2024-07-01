import QtQuick 6.2
import QtQuick.Controls
import application 1.0

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
        target: AppData.serverProcess
        onOutputChanged: {
            listModel.append({
                                 "content": AppData.serverProcess.output
                             })
            view.positionViewAtEnd()
        }
    }
}
