import QtQuick 6.2
import application 1.0

ListModel {
    id: listModel

    function appendElement(element) {
        listModel.append({
                             "workshopID": element.workshopID,
                             "mapName": element.mapName,
                             "downloadedAt": element.downloadedAt,
                             "previewPath": new URL(element.previewPath)
                         })
    }

    Component.onCompleted: {
        AppData.mapHistory.get().forEach(appendElement)
    }

    Connections {
        target: AppData.mapHistory
        onEntryAdded: {
            listModel.clear()
            AppData.mapHistory.get().forEach(appendElement)
        }
    }
}
