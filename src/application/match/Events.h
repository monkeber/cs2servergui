#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

struct MatchStarted
{
	std::string m_mapName;
};

struct PlayerStats
{
	//! Steam account id, bots report 0.
	std::uint32_t m_accountId{ 0 };
	//! 2 is T, 3 is CT.
	int m_team{ 0 };
	int m_kills{ 0 };
	int m_deaths{ 0 };
	int m_assists{ 0 };
	int m_damage{ 0 };
	double m_headshotPercent{ 0.0 };
	double m_adr{ 0.0 };
	int m_mvp{ 0 };
};

//! Snapshot the server logs at the end of every round. Player stats are running totals for the
//! whole match, so the latest snapshot replaces the previous one.
struct RoundStats
{
	std::string m_mapName;
	int m_roundNumber{ 0 };
	int m_scoreCt{ 0 };
	int m_scoreT{ 0 };
	std::vector<PlayerStats> m_players;
};

struct MatchEnded
{
};

using MatchEvent = std::variant<MatchStarted, RoundStats, MatchEnded>;
