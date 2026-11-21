#include "StatsService.h"

MatchStatsService::MatchStatsService(MatchLogParser& parser, MatchStatsStore& store)
	: m_parser{ parser }
	, m_store{ store }
{
}

void MatchStatsService::HandleLog(std::string_view chunk)
{
	for (const auto& event : m_parser.Feed(chunk))
	{
		std::visit(
			[this](const auto& e) {
				Handle(e);
			},
			event);
	}
}

void MatchStatsService::Reset()
{
	m_parser.Reset();
	m_currentMatch.reset();
}

void MatchStatsService::Handle(const MatchStarted&)
{
	// The match is stored with its first round, so a restart before any round leaves no record.
	m_currentMatch.reset();
}

void MatchStatsService::Handle(const RoundStats& event)
{
	// Covers a missed Match_Start, e.g. a map change or a game restart.
	if (m_currentMatch && !BelongsToCurrentMatch(event))
	{
		m_currentMatch.reset();
	}

	if (!m_currentMatch)
	{
		m_currentMatch = CurrentMatch{
			.m_id = m_store.CreateMatch(event.m_map),
			.m_mapName = event.m_map.m_name,
		};
	}

	// Bots all share account id 0, so they cannot be told apart.
	RoundStats realPlayersOnly{ event };
	std::erase_if(realPlayersOnly.m_players, [](const PlayerStats& player) {
		return player.m_accountId == 0;
	});

	m_store.SaveRound(m_currentMatch->m_id, realPlayersOnly);
	m_currentMatch->m_lastRound = event.m_roundNumber;
}

void MatchStatsService::Handle(const MatchEnded&)
{
	if (m_currentMatch)
	{
		m_store.EndMatch(m_currentMatch->m_id);
		m_currentMatch.reset();
	}
}

bool MatchStatsService::BelongsToCurrentMatch(const RoundStats& event) const
{
	return event.m_map.m_name == m_currentMatch->m_mapName
		&& event.m_roundNumber > m_currentMatch->m_lastRound;
}
