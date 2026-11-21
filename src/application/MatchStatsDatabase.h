#pragma once

#include "MatchEvents.h"

#include <SQLiteCpp/SQLiteCpp.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

struct MatchRecord
{
	std::int64_t m_id{ 0 };
	std::string m_mapName;
	std::optional<std::string> m_workshopId;
	std::string m_startedAt;
	std::optional<std::string> m_endedAt;
	int m_scoreCt{ 0 };
	int m_scoreT{ 0 };
};

class MatchStatsDatabase
{
public:
	//! Default constructor to use in the production code.
	MatchStatsDatabase();
	//! This constructor is mostly intended for tests.
	explicit MatchStatsDatabase(const std::string& dbName);

public:
	//! Creates a match record and returns its id.
	std::int64_t CreateMatch(const std::string& mapName) const;
	//! Stores the round snapshot: updates the match score and replaces player totals. Bots are
	//! skipped, they all share account id 0.
	void SaveRound(std::int64_t matchId, const RoundStats& stats) const;
	void EndMatch(std::int64_t matchId) const;

	std::vector<MatchRecord> SelectMatches() const;
	std::vector<PlayerStats> SelectPlayerStats(std::int64_t matchId) const;

private:
	void InitSchema() const;

private:
	std::unique_ptr<SQLite::Database> m_db;
};
