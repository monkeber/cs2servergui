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

signals:
	//! Remove 'count' map entries starting at rowIndex.
	void removeMapEntries(const int rowIndex, const int count);

public slots:
	//! Adds a row into the model data.
	void AddEntry(const MapHistoryEntry& entry);
	//! Clears all the rows from the model.
	void ClearModel();

public:
	int columnCount(const QModelIndex& = QModelIndex()) const override;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	int rowCount(const QModelIndex& = QModelIndex()) const override;

	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(
		int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

private:
	//! Translates the row number in model into entry index in the data list. It is moved into
	//! standalone method so we can, for example, use reverse order for displaying while data will
	//! be stored in the chronological ascending order.
	int GetDataIndex(const int rowNumber) const;
	//! Returns where the display order is reversed.
	bool IsDisplayOrderReversed() const;

private:
	//! Specifies whether we should display the model data in reversed order.
	bool m_displayOrderIsReversed;
	//! The list of hisotry records.
	QList<MapHistoryEntry> m_history;
};
