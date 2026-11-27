#include "MatchLogSplitter.h"

#include <optional>

std::vector<MatchLogSplitter::Piece> MatchLogSplitter::Split(std::string_view chunk)
{
	const std::string text{ m_held + std::string{ chunk } };
	bool pieceStartsNewMatch{ m_heldStartsNewMatch };
	m_held.clear();
	m_heldStartsNewMatch = false;

	std::vector<Piece> pieces;
	std::size_t pieceStart{ 0 };
	// Start of the `round_stats` block the current line is in, if it is not complete yet.
	std::optional<std::size_t> blockStart;

	std::size_t lineStart{ 0 };
	while (lineStart < text.size())
	{
		auto lineEnd{ text.find('\n', lineStart) };
		if (lineEnd == std::string::npos)
		{
			lineEnd = text.size();
		}
		const std::string_view line{ std::string_view{ text }.substr(lineStart, lineEnd - lineStart) };

		if (line.contains(R"(World triggered "Round_Start")"))
		{
			m_hasPlayedRound = true;
		}
		else if (line.contains("JSON_BEGIN"))
		{
			blockStart = lineStart;
		}
		else if (line.contains("JSON_END"))
		{
			blockStart.reset();
		}
		else if (blockStart && m_hasPlayedRound && line.contains(R"("round_number" : "1")"))
		{
			if (*blockStart > pieceStart)
			{
				pieces.push_back(
					{ text.substr(pieceStart, *blockStart - pieceStart), pieceStartsNewMatch });
			}
			pieceStart = *blockStart;
			pieceStartsNewMatch = true;
			m_hasPlayedRound = false;
		}

		lineStart = lineEnd + 1;
	}

	// The cut depends on the round number, which may still be to come.
	std::size_t end{ text.size() };
	if (blockStart)
	{
		end = *blockStart;
		m_held = text.substr(end);
		m_heldStartsNewMatch = pieceStartsNewMatch && pieceStart == end;
	}

	if (end > pieceStart)
	{
		pieces.push_back({ text.substr(pieceStart, end - pieceStart), pieceStartsNewMatch });
	}
	return pieces;
}

std::vector<MatchLogSplitter::Piece> MatchLogSplitter::Flush()
{
	std::vector<Piece> pieces;
	if (!m_held.empty())
	{
		pieces.push_back({ std::move(m_held), m_heldStartsNewMatch });
	}
	m_held.clear();
	m_heldStartsNewMatch = false;
	return pieces;
}

void MatchLogSplitter::Reset()
{
	m_held.clear();
	m_heldStartsNewMatch = false;
	// A match of the previous server session is over, whatever it logged.
	m_hasPlayedRound = true;
}
