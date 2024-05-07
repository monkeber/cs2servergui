import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts

Item {
    // Allows embedding  components into Spoiler.
    default property alias contents: mainRow.sourceComponent

    // Bind height of the main content element to the whole component.
    height: mainContent.height

    Rectangle {
        id: background
        anchors.fill: parent
        color: "transparent"
        border.color: "black"
        border.width: 5
    }

    Column {
        id: mainContent
        anchors.right: parent.right
        anchors.left: parent.left

        property bool is_expanded: false

        Button {
            id: hideToggle
            anchors.right: mainContent.right
            text: "Hidden"
            onClicked: mainContent.is_expanded = !mainContent.is_expanded
        }

        Item {
            id: info
            height: mainContent.is_expanded ? mainRow.height : 0
            anchors.right: parent.right
            anchors.left: parent.left
            clip: true
            Loader {
                id: mainRow
                anchors.right: parent.right
                anchors.left: parent.left
                visible: mainContent.is_expanded
            }
        }

        states: [
            State {
                name: "expanded"
                when: mainContent.is_expanded
                PropertyChanges {
                    target: hideToggle
                    text: "Expanded"
                }
            }
        ]
    }
}
