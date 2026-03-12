#pragma once

#include <qqml.h>
#include <QAbstractTableModel>

#include "MapHistoryDatabase.h"

class MapHistoryModel : public QAbstractTableModel
{
	Q_OBJECT
	QML_ELEMENT

public:
	MapHistoryModel();

signals:
	void RemoveMapEntry(const std::string& workshopId, const std::string& playedAt);
	void UpdateBookmarkedSignal(const std::string& workshopId, const bool isBookmarked) const;
	void UpdateFilters(
		const bool sortByRating, const bool removeDuplicated, const bool showOnlyBookmarks) const;
	void UpdateRatingSignal(const std::string& workshopId, const std::uint8_t rating) const;

public slots:
	//! Adds rows into the model data.
	void AddEntries(const std::vector<MapHistoryEntry>& entries);
	//! Clears all the rows from the model.
	void ClearModel();

public:
	Q_INVOKABLE void UpdateBookmarked(const int row, const bool rating) const;
	Q_INVOKABLE void UpdateRating(const int row, const std::uint8_t isBookmarked) const;

	int columnCount(const QModelIndex& = QModelIndex()) const override;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	int rowCount(const QModelIndex& = QModelIndex()) const override;

	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(
		int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

public:
	enum class Columns
	{
		MapWorkshopId = 0,
		PlayedAt,
		MapName,
		Rating,
		Bookmarked,
		Preview
	};
	Q_ENUM(Columns)

private:
	//! The list of history records.
	QList<MapHistoryEntry> m_history;
};
