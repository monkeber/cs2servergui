#pragma once

#include "Events.h"

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

//! Turns the raw CS2 server log stream into match events. Log text may arrive in arbitrary
//! chunks, so incomplete lines and unfinished JSON blocks are kept until the next call.
class MatchLogParser
{
public:
	std::vector<MatchEvent> Feed(std::string_view chunk);

private:
	std::optional<MatchEvent> ParseLine(std::string_view line);
	std::optional<MatchEvent> FinishJsonBlock();

private:
	std::string m_pendingLine;
	bool m_isInsideJson{ false };
	//! Key-value pairs of the JSON block being read, nested "players" entries included.
	std::map<std::string, std::string, std::less<>> m_jsonValues;
};
