#pragma once

#include <qqml.h>
#include <QAbstractTableModel>

struct MapHistoryEntry
{
public:
	QString m_workshopID;
	QString m_mapName;
	QString m_downloadedAt;
	QString m_previewPath;
};

class MapHistoryModel : public QAbstractTableModel
{
	Q_OBJECT
	QML_ELEMENT

public:
	MapHistoryModel();
	void AddEntry(const MapHistoryEntry& entry);

	int columnCount(const QModelIndex& = QModelIndex()) const override;
	int rowCount(const QModelIndex& = QModelIndex()) const override;

	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(
		int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

private:
	QList<MapHistoryEntry> m_history;
};
