#include "MapHistoryModel.h"

MapHistoryModel::MapHistoryModel()
	: m_displayOrderIsReversed{ true }
{
}

void MapHistoryModel::AddEntry(const MapHistoryEntry& entry)
{
	beginInsertRows(QModelIndex{}, m_history.size(), m_history.size());
	m_history.append(entry);
	endInsertRows();
}

void MapHistoryModel::ClearModel()
{
	beginResetModel();
	m_history.clear();
	endResetModel();
}

int MapHistoryModel::columnCount(const QModelIndex& index) const
{
	// The number of members in MapHistoryEntry struct.
	Q_UNUSED(index);
	return 4;
}

bool MapHistoryModel::removeRows(int row, int count, const QModelIndex& parent)
{
	beginRemoveRows(parent, row, row + count - 1);
	if (IsDisplayOrderReversed())
	{
		// +1 because erase() deletes the first element in the specified range and does not delete
		// the last, while in this case we want the opposite.
		const int endIndex{ GetDataIndex(row) + 1 };
		const int beginIndex{ GetDataIndex(row) - count + 1 };

		m_history.erase(m_history.cbegin() + beginIndex, m_history.cbegin() + endIndex);
		emit removeMapEntries(beginIndex, count);
	}
	else
	{
		m_history.erase(m_history.cbegin() + row, m_history.cbegin() + count);
		emit removeMapEntries(row, count);
	}
	endRemoveRows();
	return true;
}

int MapHistoryModel::rowCount(const QModelIndex& index) const
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

	switch (role)
	{
	case Qt::DisplayRole: {
		// We will show the rows in reverse order so the new maps will appear on top for
		// convenience.
		const auto entry = m_history.at(GetDataIndex(index.row()));
		switch (index.column())
		{
		case 0:
			return entry.m_workshopID;
		case 1:
			return entry.m_downloadedAt;
		case 2:
			return entry.m_mapName;
		case 3:
			return entry.m_previewPath;
		default:
			return QString{ "Column index is not handled" };
		}
	}
	default:
		break;
	}

	return QVariant();
}

QVariant MapHistoryModel::headerData(int section, Qt::Orientation orientation, int) const
{
	if (orientation == Qt::Orientation::Vertical)
	{
		return "Undefined";
	}

	switch (section)
	{
	case 0:
		return "Workshop ID";
	case 1:
		return "Downloaded At";
	case 2:
		return "Map Name";
	case 3:
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

int MapHistoryModel::GetDataIndex(const int rowNumber) const
{
	if (IsDisplayOrderReversed())
	{
		return m_history.size() - rowNumber - 1;
	}

	return rowNumber;
}

bool MapHistoryModel::IsDisplayOrderReversed() const
{
	return m_displayOrderIsReversed;
}
