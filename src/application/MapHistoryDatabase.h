#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

struct MapHistoryEntry
{
	std::string m_workshopID;
	std::string m_mapName;
	std::string m_playedAt;
	std::string m_previewPath;
	bool m_isBookmarked{ false };
	std::uint8_t m_rating{ 0 };
};

struct MapHistoryPatch
{
	std::optional<bool> m_isBookmarked;
	std::optional<std::uint8_t> m_rating;
};

class MapHistoryDatabase
{
public:
	//! Default constructor to use in the production code.
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
	//! Deletes the entry with matching map id and played at. May also delete the map info
	//! completely if there is no recorded sessions available.
	void Delete(const std::string& mapId, const std::string& playedAt) const;
	//! Returns true if the record of a map with the provided id exists in the database.
	bool Exists(const std::string& mapId) const;
	//! Returns all the stored records of the played maps.
	std::vector<MapHistoryEntry> Select(const bool sortByRating, const bool removeDuplicated, const bool showOnlyBookmarks) const;
	//! Returns the entry according to the provided workshop id and played at.
	MapHistoryEntry SelectOne(const std::string& mapId, const std::string& playedAt) const;
	//! Updates is_bookmarked or rating field of the entry, workshop id serves as the identification
	//! of the map.
	void Update(const std::string& workshopId, const MapHistoryPatch& newData) const;

private:
	//! Initializes the schema of the DB.
	void InitSchema();

private:
	//! Name of the file / DB name to be able to create and backup the DB properly.
	std::string m_dbName;
	//! Handle of the sqlite DB.
	std::unique_ptr<SQLite::Database> m_db;
};
