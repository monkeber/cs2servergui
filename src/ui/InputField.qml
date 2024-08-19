import QtQuick 6.2
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    property string text
    property alias input: input

    Label {
        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

        font: Globals.font
        text: parent.text + ":"
    }
    TextField {
        id: input

        Layout.fillWidth: true
        Layout.fillHeight: true

        font: Globals.font
    }
}
