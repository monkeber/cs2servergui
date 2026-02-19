#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

class Database
{
public:
	Database();
	~Database();

public:
	//! Adds just a new entry to map history for already existing map id.
	void AddJournalEntry(const std::string& mapId);
	//! Adds info about the new map.
	void AddNewMap(
		const std::string& mapId, const std::string& mapName, const std::string& previewPath);
	//! Returns true if the record of a map with the provided id exists in the database.
	bool Exists(const std::string& mapId) const;

private:
	void InitSchema();

private:
	std::unique_ptr<SQLite::Database> m_db;
};
