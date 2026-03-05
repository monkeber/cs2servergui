#include "MapHistoryDatabase.h"

#include <QtLogging>

namespace
{
namespace details
{

void ExtractColumn(SQLite::Statement& query, const std::string_view name, auto& field)
{
	if (!query.isColumnNull(name.data()))
	{
		if constexpr (std::is_same_v<decltype(field), std::string&>)
		{
			field = query.getColumn(name.data()).getString();
		}
		else if constexpr (std::is_same_v<decltype(field), bool&>)
		{
			field = static_cast<bool>(query.getColumn(name.data()).getInt());
		}
		else if constexpr (std::is_integral_v<std::remove_reference_t<decltype(field)>>)
		{
			field = query.getColumn(name.data()).getUInt();
		}
	}
}

MapHistoryEntry ExtractData(SQLite::Statement& query)
{
	MapHistoryEntry entry;
	details::ExtractColumn(query, "workshop_id", entry.m_workshopID);
	details::ExtractColumn(query, "played_at", entry.m_playedAt);
	details::ExtractColumn(query, "name", entry.m_mapName);
	details::ExtractColumn(query, "preview_path", entry.m_previewPath);
	details::ExtractColumn(query, "rating", entry.m_rating);
	details::ExtractColumn(query, "is_bookmarked", entry.m_isBookmarked);

	return entry;
}

}	 // namespace details
}	 // namespace

MapHistoryDatabase::MapHistoryDatabase()
	: MapHistoryDatabase{ "map_history.db3" }
{
}

MapHistoryDatabase::MapHistoryDatabase(const std::string& dbName)
	: m_dbName{ dbName }
	, m_db{
		std::make_unique<SQLite::Database>(dbName, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE),
	}
{
	// Check if the file is empty, we can implement some kind of versioning later if needed.
	if (!m_db->tableExists("version_history"))
	{
		InitSchema();
	}
}

MapHistoryDatabase::~MapHistoryDatabase()
{
	m_db->backup(m_dbName.c_str(), SQLite::Database::BackupType::Save);
}

void MapHistoryDatabase::AddJournalEntry(const std::string& mapId) const
{
	SQLite::Transaction transaction{ *m_db };

	const std::string query{ std::format(
		"INSERT INTO session_history (workshop_id) VALUES ({});", mapId) };
	m_db->exec(query);

	transaction.commit();
}

void MapHistoryDatabase::AddNewMap(
	const std::string& mapId, const std::string& mapName, const std::string& previewPath) const
{
	SQLite::Transaction transaction{ *m_db };

	const std::string query{ std::format(R"__(
	INSERT INTO maps (workshop_id, name, preview_path)
		VALUES ({0}, '{1}', '{2}');
	
	INSERT INTO session_history (workshop_id) VALUES ({0});
	)__",
		mapId,
		mapName,
		previewPath) };

	m_db->exec(query);
	transaction.commit();
}

void MapHistoryDatabase::Delete(const std::string& mapId, const std::string& playedAt) const
{
	SQLite::Transaction transaction{ *m_db };

	m_db->exec(
		std::format("DELETE FROM session_history WHERE workshop_id = {} AND played_at = '{}';",
			mapId,
			playedAt));
	transaction.commit();
}

bool MapHistoryDatabase::Exists(const std::string& mapId) const
{
	SQLite::Transaction transaction{ *m_db };

	const std::string query{ std::format(
		"SELECT EXISTS(SELECT 1 FROM maps WHERE workshop_id = {});", mapId) };

	return static_cast<bool>(m_db->execAndGet(query).getInt());
}

std::vector<MapHistoryEntry> MapHistoryDatabase::Select(
	const bool sortByRating, const bool removeDuplicated, const bool showOnlyBookmarks) const
{
	std::string queryStr{ std::format(
		"SELECT * FROM map_history_data {} ORDER BY played_at {} DESC;",
		showOnlyBookmarks ? "WHERE is_bookmarked = TRUE" : "",
		sortByRating ? ", rating" : "") };

	SQLite::Transaction transaction{ *m_db };
	SQLite::Statement query{ *m_db, queryStr };

	std::vector<MapHistoryEntry> result;
	while (query.executeStep())
	{
		result.push_back(details::ExtractData(query));
	}

	return result;
}

MapHistoryEntry MapHistoryDatabase::SelectOne(
	const std::string& mapId, const std::string& playedAt) const
{
	SQLite::Transaction transaction{ *m_db };
	SQLite::Statement query{ *m_db,
		std::format("SELECT * FROM map_history_data WHERE workshop_id = {} AND played_at = '{}';",
			mapId,
			playedAt) };

	if (query.executeStep())
	{
		return details::ExtractData(query);
	}

	throw std::runtime_error{ std::format(
		"No entry with specified workshop id and played timestamp: {} {}", mapId, playedAt) };
}

void MapHistoryDatabase::Update(const std::string& workshopId, const MapHistoryPatch& newData) const
{
	if (workshopId.empty())
	{
		throw std::runtime_error{ "Cannot update DB record due to missing workshop ID" };
	}

	std::vector<std::string> updates;

	if (newData.m_isBookmarked.has_value())
	{
		updates.push_back(std::format("is_bookmarked = {}", newData.m_isBookmarked.value()));
	}
	if (newData.m_rating.has_value())
	{
		updates.push_back(std::format("rating = {}", newData.m_rating.value()));
	}

	if (updates.empty())
	{
		return;
	}

	std::string updateClause;
	for (const auto& upd : updates)
	{
		if (updateClause.empty())
		{
			updateClause = upd;
		}
		else
		{
			updateClause += std::format(", {}", upd);
		}
	}

	SQLite::Transaction transaction{ *m_db };
	m_db->exec(std::format("UPDATE maps SET {} WHERE workshop_id = {}", updateClause, workshopId));
	transaction.commit();
}

void MapHistoryDatabase::InitSchema()
{
	SQLite::Transaction transaction{ *m_db };
	m_db->exec(R"__(
	CREATE TABLE version_history(
		version INTEGER NOT NULL
	);

	INSERT INTO version_history(version)
		VALUES(2026021800);

	CREATE TABLE maps(
		workshop_id INTEGER PRIMARY KEY NOT NULL,
		name TEXT NOT NULL DEFAULT '<unnamed>',
		preview_path TEXT,
		rating INTEGER NOT NULL DEFAULT 0 CHECK (rating >= 0 AND rating <= 5),
		is_bookmarked BOOL NOT NULL DEFAULT FALSE
	);

	-- I couldn't mark the pair as unique because sqlite does not have enough time precision (milliseconds) for testing.
	CREATE TABLE session_history(
		workshop_id INTEGER NOT NULL REFERENCES maps(workshop_id),
		played_at DATETIME NOT NULL DEFAULT(datetime('subsec')),
		UNIQUE(workshop_id,played_at)
	);

	CREATE VIEW map_history_data AS
		SELECT maps.workshop_id, name, preview_path, rating, is_bookmarked, played_at
		FROM session_history LEFT JOIN maps ON maps.workshop_id = session_history.workshop_id
		ORDER BY played_at DESC
	;

	-- Delete the map record if the last recorded session has been deleted.
	CREATE TRIGGER delete_map_info
	AFTER DELETE ON session_history
	WHEN NOT EXISTS (SELECT 1 FROM session_history WHERE workshop_id = OLD.workshop_id)
	BEGIN
		DELETE FROM maps WHERE workshop_id = OLD.workshop_id;
	END;
	)__");

	transaction.commit();
}
