#include "MapHistoryModel.h"

MapHistoryModel::MapHistoryModel()
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

int MapHistoryModel::rowCount(const QModelIndex& index) const
{
	return m_history.size();
}

int MapHistoryModel::columnCount(const QModelIndex& index) const
{
	// The number of members in MapHistoryEntry struct.
	Q_UNUSED(index);
	return 4;
}

QVariant MapHistoryModel::data(const QModelIndex& index, int role) const
{
	switch (role)
	{
	case Qt::DisplayRole: {
		const auto entry = m_history.at(index.row());
		switch (index.column())
		{
		case 0:
			return entry.m_workshopID;
		case 1:
			return entry.m_mapName;
		case 2:
			return entry.m_downloadedAt;
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
		return "Map Name";
	case 2:
		return "Downloaded At";
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
