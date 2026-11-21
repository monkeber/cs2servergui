#pragma once

#include "LogParser.h"

#include <map>
#include <optional>
#include <string>

//! Parses the CS2 dedicated server log: match start and end lines, and the `round_stats` JSON
//! block the server writes at the end of every round.
class Cs2MatchLogParser final : public MatchLogParser
{
public:
	std::vector<MatchEvent> Feed(std::string_view chunk) override;
	void Reset() override;

private:
	void ParseLine(std::string_view line, std::vector<MatchEvent>& events);
	std::optional<MatchEvent> FinishJsonBlock();

private:
	std::string m_pendingLine;
	bool m_isInsideJson{ false };
	//! Key-value pairs of the JSON block being read, nested "players" entries included.
	std::map<std::string, std::string, std::less<>> m_jsonValues;
};
