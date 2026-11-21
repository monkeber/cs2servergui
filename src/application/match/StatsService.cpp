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

void MatchStatsService::Handle(const MatchStarted& event)
{
	m_currentMatchId = m_store.CreateMatch(event.m_map);
}

void MatchStatsService::Handle(const RoundStats& event)
{
	// The app may start listening in the middle of a match, the round snapshot still has the
	// whole match totals.
	if (!m_currentMatchId)
	{
		m_currentMatchId = m_store.CreateMatch(event.m_map);
	}

	// Bots all share account id 0, so they cannot be told apart.
	RoundStats realPlayersOnly{ event };
	std::erase_if(realPlayersOnly.m_players, [](const PlayerStats& player) {
		return player.m_accountId == 0;
	});

	m_store.SaveRound(*m_currentMatchId, realPlayersOnly);
}

void MatchStatsService::Handle(const MatchEnded&)
{
	if (m_currentMatchId)
	{
		m_store.EndMatch(*m_currentMatchId);
		m_currentMatchId.reset();
	}
}
