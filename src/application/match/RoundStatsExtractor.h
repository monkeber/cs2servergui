#pragma once

#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

//! Turns the `round_stats` blocks of the log stream into one line each, to be added to the log.
//!
//! The game logs a block as many log lines (`JSON_BEGIN`, one line per key and per player,
//! `JSON_END`). The extra line makes it easy to find and parse:
//!
//!   `<time prefix> ROUND_STATS {"name":"round_stats","round_number":15,...,"players":[{"userid":2,"accountid":0,"team":2,...}]}`
//!
//! After `Game Over` it also gives a note saying whether a block was found for every round.
class RoundStatsExtractor
{
public:
	//! Chunks may end in the middle of a line, the rest of it is expected in the next chunk.
	//! Returns the lines to add after the chunk, in order, without a trailing newline.
	std::vector<std::string> Feed(std::string_view chunk);

	//! A new match starts, the rounds of the previous one are forgotten.
	void StartNewMatch();
	//! For when the game server starts or stops, nothing of the previous session is continued.
	void Reset();

private:
	void HandleLine(std::string_view line, std::vector<std::string>& output);
	std::string RoundStatsLine();
	std::string CheckAllRoundsSeen(std::string_view gameOverLine);

private:
	std::string m_partialLine;

	bool m_isInsideJson{ false };
	//! Text in front of `JSON_BEGIN`, the date and time of the block.
	std::string m_blockPrefix;
	//! Keys and values of the lines of the block that is being read, in the order of the log.
	std::vector<std::pair<std::string, std::string>> m_blockValues;
	//! Round numbers of the `round_stats` blocks of the current match.
	std::set<int> m_seenRounds;
};
