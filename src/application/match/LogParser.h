#pragma once

#include "Events.h"

#include <string_view>
#include <vector>

//! Turns raw log text into match events. Log text may arrive in arbitrary chunks, so
//! implementations keep incomplete data until the next call.
class MatchLogParser
{
public:
	virtual ~MatchLogParser() = default;

	virtual std::vector<MatchEvent> Feed(std::string_view chunk) = 0;
	//! Drops incomplete data, for when the log stream starts over.
	virtual void Reset() = 0;
};
