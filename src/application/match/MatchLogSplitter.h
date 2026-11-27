#pragma once

#include <string>
#include <string_view>
#include <vector>

//! Finds where a new match starts in the log stream, so every match can get its own file.
//!
//! The game logs the `round_stats` block of round 1 *before* the `Match_Start` line, so a match
//! starts with the `JSON_BEGIN` line of the block with `round_number` 1. That only counts as a new
//! match once a round was played since the previous one, `Match_Start` and the block are logged
//! again on a restart during warmup.
class MatchLogSplitter
{
public:
	struct Piece
	{
		std::string m_text;
		//! The piece is the beginning of a new match, so it goes to a new file.
		bool m_startsNewMatch{ false };
	};

	//! Cuts the chunk in front of every block that starts a new match. A block that is not
	//! complete at the end of the chunk is held back until the next chunk, as the cut depends on
	//! its round number. May return no pieces.
	std::vector<Piece> Split(std::string_view chunk);

	//! Returns what is still held back, for when no more of the log is expected.
	std::vector<Piece> Flush();

	//! For when the game server starts or stops, the next match starts a new file. Call `Flush`
	//! first, the held back text is dropped.
	void Reset();

private:
	bool m_hasPlayedRound{ false };
	std::string m_held;
	bool m_heldStartsNewMatch{ false };
};
