#pragma once

#include "LogParser.h"
#include "StatsStore.h"

#include <cstdint>
#include <optional>
#include <string_view>

//! Tracks the current match and stores the stats of real players from the log stream.
class MatchStatsService
{
public:
	MatchStatsService(MatchLogParser& parser, MatchStatsStore& store);

public:
	void HandleLog(std::string_view chunk);

private:
	void Handle(const MatchStarted& event);
	void Handle(const RoundStats& event);
	void Handle(const MatchEnded& event);

private:
	MatchLogParser& m_parser;
	MatchStatsStore& m_store;
	std::optional<std::int64_t> m_currentMatchId;
};
