#pragma once

#include "StatsStore.h"

#include <SQLiteCpp/SQLiteCpp.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

struct MatchRecord
{
	std::int64_t m_id{ 0 };
	MapInfo m_map;
	std::string m_startedAt;
	std::optional<std::string> m_endedAt;
	int m_scoreCt{ 0 };
	int m_scoreT{ 0 };
};

class SqliteMatchStatsStore final : public MatchStatsStore
{
public:
	//! Default constructor to use in the production code.
	SqliteMatchStatsStore();
	//! This constructor is mostly intended for tests.
	explicit SqliteMatchStatsStore(const std::string& dbName);

public:
	std::int64_t CreateMatch(const MapInfo& map) override;
	void SaveRound(std::int64_t matchId, const RoundStats& stats) override;
	void EndMatch(std::int64_t matchId) override;

	std::vector<MatchRecord> SelectMatches() const;
	std::vector<PlayerStats> SelectPlayerStats(std::int64_t matchId) const;

private:
	void InitSchema() const;

private:
	std::unique_ptr<SQLite::Database> m_db;
};
