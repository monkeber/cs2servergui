#pragma once

#include "Events.h"

#include <cstdint>

//! Persists match stats.
class MatchStatsStore
{
public:
	virtual ~MatchStatsStore() = default;

	//! Creates a match record and returns its id.
	virtual std::int64_t CreateMatch(const MapInfo& map) = 0;
	//! Updates the match score and replaces the player totals with the ones from the snapshot.
	virtual void SaveRound(std::int64_t matchId, const RoundStats& stats) = 0;
	virtual void EndMatch(std::int64_t matchId) = 0;
};
