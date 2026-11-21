#include "StatsService.h"

MatchStatsService::MatchStatsService(const MatchStatsDatabase& db)
	: m_db{ db }
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
	m_currentMatchId = m_db.CreateMatch(event.m_mapName);
}

void MatchStatsService::Handle(const RoundStats& event)
{
	// The app may start listening in the middle of a match, the round snapshot still has the
	// whole match totals.
	if (!m_currentMatchId)
	{
		m_currentMatchId = m_db.CreateMatch(event.m_mapName);
	}

	m_db.SaveRound(*m_currentMatchId, event);
}

void MatchStatsService::Handle(const MatchEnded&)
{
	if (m_currentMatchId)
	{
		m_db.EndMatch(*m_currentMatchId);
		m_currentMatchId.reset();
	}
}
