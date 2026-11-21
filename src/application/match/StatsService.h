#pragma once

#include "LogParser.h"
#include "StatsDatabase.h"

#include <cstdint>
#include <optional>
#include <string_view>

//! Turns the server log stream into stored match stats.
class MatchStatsService
{
public:
	explicit MatchStatsService(const MatchStatsDatabase& db);

public:
	void HandleLog(std::string_view chunk);

private:
	void Handle(const MatchStarted& event);
	void Handle(const RoundStats& event);
	void Handle(const MatchEnded& event);

private:
	const MatchStatsDatabase& m_db;
	MatchLogParser m_parser;
	std::optional<std::int64_t> m_currentMatchId;
};
