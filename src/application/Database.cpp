#include "Database.h"

Database::Database()
	: m_db{
		std::make_unique<SQLite::Database>(
			"map_history.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE),
	}
{
	// Check if it was the file is empty, we can implement some kind of versioning later if needed.
	if (!m_db->tableExists("version_history"))
	{
		InitSchema();
	}
}

Database::~Database()
{
	m_db->backup("map_history.db3", SQLite::Database::BackupType::Save);
}

void Database::AddJournalEntry(const std::string& mapId)
{
	SQLite::Transaction transaction{ *m_db };
	SQLite::Statement query{ *m_db, R"__(
	INSERT INTO session_history (workshop_id) VALUES (?);
	)__" };

	query.bind(mapId);
	query.exec();
	transaction.commit();
}

void Database::AddNewMap(
	const std::string& mapId, const std::string& mapName, const std::string& previewPath)
{
	SQLite::Transaction transaction{ *m_db };
	SQLite::Statement query{ *m_db, R"__(
	INSERT INTO maps (workshop_id, name, preview_path)
		VALUES (?1, '?2', '?3');
	
	INSERT INTO session_history (workshop_id) VALUES (?1);
	)__" };

	query.bind(1, mapId);
	query.bind(2, mapName);
	query.bind(3, previewPath);
	query.exec();
	transaction.commit();
}

bool Database::Exists(const std::string& mapId) const
{
	SQLite::Transaction transaction{ *m_db };
	SQLite::Statement query{ *m_db, R"__(
	SELECT EXISTS(SELECT 1 FROM maps WHERE workshop_id = ?);
	)__" };

	query.bind(mapId);
	query.exec();

	return static_cast<bool>(query.getColumn(0).getInt());
}

void Database::InitSchema()
{
	SQLite::Transaction transaction{ *m_db };
	SQLite::Statement query{ *m_db, R"__(
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

	CREATE TABLE session_history(
		workshop_id INTEGER NOT NULL REFERENCES maps(workshop_id),
		played_at NUMERIC NOT NULL DEFAULT current_timestamp,
		UNIQUE (workshop_id, played_at)
	);

	CREATE VIEW map_history_data AS
		SELECT workshop_id, name, preview_path, rating, is_bookmarked, played_at
		FROM session_history LEFT JOIN maps ON maps.workshop_id = session_history.workshop_id
		ORDER BY played_at DESC
	;
	)__" };

	query.exec();
	transaction.commit();
}
