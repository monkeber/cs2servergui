#pragma once

#include "LogParser.h"
#include "StatsStore.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

//! Tracks the current match and stores the stats of real players from the log stream. A match is
//! stored with its first round, and one that never reaches Game Over keeps no end time.
class MatchStatsService
{
public:
	MatchStatsService(MatchLogParser& parser, MatchStatsStore& store);

public:
	void HandleLog(std::string_view chunk);
	//! Starts over, for when the game server starts or stops.
	void Reset();

private:
	void Handle(const MatchStarted& event);
	void Handle(const RoundStats& event);
	void Handle(const MatchEnded& event);
	void Handle(const PlayerSeen& event);

	bool BelongsToCurrentMatch(const RoundStats& event) const;

private:
	struct CurrentMatch
	{
		std::int64_t m_id{ 0 };
		std::string m_mapName;
		int m_lastRound{ 0 };
	};

	MatchLogParser& m_parser;
	MatchStatsStore& m_store;
	std::optional<CurrentMatch> m_currentMatch;
	//! Names already stored, so every log line with a player does not hit the store.
	std::unordered_map<std::uint32_t, std::string> m_knownNames;
};
