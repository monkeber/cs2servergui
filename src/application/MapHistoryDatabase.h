#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

struct MapHistoryEntry
{
public:
	std::string m_workshopID;
	std::string m_mapName;
	std::string m_playedAt;
	std::string m_previewPath;
	bool m_isBookmarked{ false };
	std::uint8_t m_rating{ 0 };
};

class MapHistoryDatabase
{
public:
	MapHistoryDatabase();
	//! This constructor is mostly intended for tests.
	explicit MapHistoryDatabase(const std::string& dbName);
	~MapHistoryDatabase();

public:
	//! Adds just a new entry to map history for already existing map id.
	void AddJournalEntry(const std::string& mapId) const;
	//! Adds info about the new map and a new entry in the history.
	void AddNewMap(
		const std::string& mapId, const std::string& mapName, const std::string& previewPath) const;
	//! Returns true if the record of a map with the provided id exists in the database.
	bool Exists(const std::string& mapId) const;
	//! Returns all the stored records of the played maps.
	std::vector<MapHistoryEntry> Select() const;

private:
	void InitSchema();

private:
	std::string m_dbName;
	std::unique_ptr<SQLite::Database> m_db;
};
