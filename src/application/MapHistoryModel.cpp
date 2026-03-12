#include "MapHistoryModel.h"

MapHistoryModel::MapHistoryModel()
{
}

void MapHistoryModel::AddEntries(const std::vector<MapHistoryEntry>& entries)
{
	beginInsertRows(QModelIndex{}, 0, entries.size());
	m_history = QList<MapHistoryEntry>{ entries.begin(), entries.end() };
	endInsertRows();
}

void MapHistoryModel::ClearModel()
{
	beginResetModel();
	m_history.clear();
	endResetModel();
}

void MapHistoryModel::UpdateBookmarked(const int row, const bool isBookmarked) const
{
	emit UpdateBookmarkedSignal(m_history[row].m_workshopID, isBookmarked);
}

void MapHistoryModel::UpdateRating(const int row, const std::uint8_t rating) const
{
	emit UpdateRatingSignal(m_history[row].m_workshopID, rating);
}

int MapHistoryModel::columnCount(const QModelIndex& index) const
{
	// The number of members in MapHistoryEntry struct.
	Q_UNUSED(index);
	return 6;
}

bool MapHistoryModel::removeRows(int row, int count, const QModelIndex& parent)
{
	// We assume the count is always 1.
	const MapHistoryEntry entry{ m_history[row] };
	beginRemoveRows(parent, row, row + count - 1);
	m_history.erase(m_history.cbegin() + row, m_history.cbegin() + row + count);
	endRemoveRows();
	emit RemoveMapEntry(entry.m_workshopID, entry.m_playedAt);
	return true;
}

int MapHistoryModel::rowCount(const QModelIndex&) const
{
	return m_history.size();
}

QVariant MapHistoryModel::data(const QModelIndex& index, int role) const
{
	// Since rows are counted from 0 then we should never have row number bigger than data size (or
	// equal to it). This check also should handle empty data as well.
	if (index.row() >= m_history.size())
	{
		return QVariant();
	}

	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}

	// We will show the rows in reverse order so the new maps will appear on top for
	// convenience.
	const auto entry = m_history.at(index.row());
	switch (static_cast<Columns>(index.column()))
	{
	case Columns::MapWorkshopId:
		return QString::fromStdString(entry.m_workshopID);
	case Columns::PlayedAt:
		return QString::fromStdString(entry.m_playedAt);
	case Columns::MapName:
		return QString::fromStdString(entry.m_mapName);
	case Columns::Rating:
		return entry.m_rating;
	case Columns::Bookmarked:
		return entry.m_isBookmarked;
	case Columns::Preview: {
		const std::filesystem::path path{ entry.m_previewPath };
		return QString{ "file:///%1" }.arg(std::filesystem::canonical(path).string().c_str());
	}
	default:
		return QString{ "Column index is not handled" };
	}
}

QVariant MapHistoryModel::headerData(int section, Qt::Orientation orientation, int) const
{
	if (orientation == Qt::Orientation::Vertical)
	{
		return "Undefined";
	}

	switch (static_cast<Columns>(section))
	{
	case Columns::MapWorkshopId:
		return "Workshop ID";
	case Columns::PlayedAt:
		return "Played At";
	case Columns::MapName:
		return "Map Name";
	case Columns::Rating:
		return "Rating";
	case Columns::Bookmarked:
		return "";
	case Columns::Preview:
		return "Preview";
	default:
		return QString{ "Column index is not handled" };
	}
}

QHash<int, QByteArray> MapHistoryModel::roleNames() const
{
	return {
		{ Qt::DisplayRole, "display" },
	};
}
