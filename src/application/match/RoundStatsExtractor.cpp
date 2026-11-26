#include "RoundStatsExtractor.h"

#include <nlohmann/json.hpp>

#include <charconv>
#include <cstdint>
#include <format>
#include <optional>

namespace
{

constexpr std::string_view kRoundStatsPrefix{ "ROUND_STATS " };

std::string_view Trim(std::string_view text)
{
	while (!text.empty() && text.front() == ' ')
	{
		text.remove_prefix(1);
	}
	while (!text.empty() && text.back() == ' ')
	{
		text.remove_suffix(1);
	}
	return text;
}

//! Number after `key`, e.g. 9 for `score 9:6` with key `score `.
std::optional<int> NumberAfter(std::string_view line, std::string_view key)
{
	const auto keyPosition{ line.find(key) };
	if (keyPosition == std::string_view::npos)
	{
		return std::nullopt;
	}

	const auto digits{ line.substr(keyPosition + key.size()) };
	int number{ 0 };
	const auto result{ std::from_chars(digits.data(), digits.data() + digits.size(), number) };
	return result.ec == std::errc{} ? std::optional{ number } : std::nullopt;
}

//! Before: `11/26/2026 - 11:56:59.478 - "round_number" : "15"`
//! After:  {"round_number", "15"}, lines without a quoted value (`"players" : {`) give nothing
std::optional<std::pair<std::string, std::string>> ParseKeyValue(std::string_view line)
{
	const auto keyOpen{ line.find('"') };
	if (keyOpen == std::string_view::npos)
	{
		return std::nullopt;
	}
	const auto keyClose{ line.find('"', keyOpen + 1) };
	if (keyClose == std::string_view::npos)
	{
		return std::nullopt;
	}
	const auto valueOpen{ line.find('"', keyClose + 1) };
	const auto valueClose{ line.rfind('"') };
	if (valueOpen == std::string_view::npos || valueClose <= valueOpen)
	{
		return std::nullopt;
	}

	return std::pair{ std::string{ line.substr(keyOpen + 1, keyClose - keyOpen - 1) },
		std::string{ line.substr(valueOpen + 1, valueClose - valueOpen - 1) } };
}

//! Numbers become JSON numbers, everything else stays a string.
nlohmann::ordered_json ToJsonValue(std::string_view text)
{
	text = Trim(text);

	std::int64_t integer{ 0 };
	if (const auto result{ std::from_chars(text.data(), text.data() + text.size(), integer) };
		result.ec == std::errc{} && result.ptr == text.data() + text.size())
	{
		return integer;
	}

	double real{ 0 };
	if (const auto result{ std::from_chars(text.data(), text.data() + text.size(), real) };
		result.ec == std::errc{} && result.ptr == text.data() + text.size())
	{
		return real;
	}

	return std::string{ text };
}

std::vector<std::string_view> SplitCsv(std::string_view text)
{
	std::vector<std::string_view> parts;
	while (true)
	{
		const auto comma{ text.find(',') };
		parts.push_back(Trim(text.substr(0, comma)));
		if (comma == std::string_view::npos)
		{
			return parts;
		}
		text.remove_prefix(comma + 1);
	}
}

}	 // namespace

std::vector<std::string> RoundStatsExtractor::Feed(std::string_view chunk)
{
	std::vector<std::string> output;
	m_partialLine += chunk;

	std::size_t lineStart{ 0 };
	for (auto lineEnd{ m_partialLine.find('\n') }; lineEnd != std::string::npos;
		 lineEnd = m_partialLine.find('\n', lineStart))
	{
		std::string_view line{ std::string_view{ m_partialLine }.substr(
			lineStart, lineEnd - lineStart) };
		if (line.ends_with('\r'))
		{
			line.remove_suffix(1);
		}
		HandleLine(line, output);
		lineStart = lineEnd + 1;
	}
	m_partialLine.erase(0, lineStart);

	return output;
}

void RoundStatsExtractor::StartNewMatch()
{
	m_seenRounds.clear();
}

void RoundStatsExtractor::Reset()
{
	// A line or block cut by the restart will never be finished.
	m_partialLine.clear();
	m_isInsideJson = false;
	m_seenRounds.clear();
}

void RoundStatsExtractor::HandleLine(std::string_view line, std::vector<std::string>& output)
{
	if (const auto begin{ line.find("JSON_BEGIN") }; begin != std::string_view::npos)
	{
		m_isInsideJson = true;
		m_blockPrefix = line.substr(0, begin);
		m_blockValues.clear();
		return;
	}

	if (m_isInsideJson)
	{
		if (line.contains("JSON_END"))
		{
			m_isInsideJson = false;
			output.push_back(RoundStatsLine());
		}
		else if (auto keyValue{ ParseKeyValue(line) })
		{
			m_blockValues.push_back(std::move(*keyValue));
		}
		return;
	}

	if (line.contains("Game Over:"))
	{
		output.push_back(CheckAllRoundsSeen(line));
	}
}

//! Before: `"round_number" : "15"`, `"fields" : "accountid, team, ..."`, `"player_2" : "0, 2, ..."`
//! After:  `<prefix> ROUND_STATS {"round_number":15,...,"players":[{"userid":2,"accountid":0,...}]}`
std::string RoundStatsExtractor::RoundStatsLine()
{
	nlohmann::ordered_json block{ nlohmann::ordered_json::object() };
	std::vector<std::string_view> columns;
	for (const auto& [key, value] : m_blockValues)
	{
		if (key == "fields")
		{
			columns = SplitCsv(value);
		}
		else if (!key.starts_with("player_"))
		{
			block[key] = ToJsonValue(value);
		}
	}

	// The columns are named by the game, so a change of their order in a game update is harmless.
	auto players{ nlohmann::ordered_json::array() };
	for (const auto& [key, value] : m_blockValues)
	{
		if (!key.starts_with("player_"))
		{
			continue;
		}

		nlohmann::ordered_json player{ nlohmann::ordered_json::object() };
		player["userid"] =
			ToJsonValue(std::string_view{ key }.substr(std::string_view{ "player_" }.size()));
		const auto values{ SplitCsv(value) };
		for (std::size_t i{ 0 }; i < columns.size() && i < values.size(); ++i)
		{
			player[std::string{ columns[i] }] = ToJsonValue(values[i]);
		}
		players.push_back(std::move(player));
	}
	block["players"] = std::move(players);

	if (block.contains("round_number") && block["round_number"].is_number_integer())
	{
		m_seenRounds.insert(block["round_number"].get<int>());
	}

	return std::format("{}{}{}", m_blockPrefix, kRoundStatsPrefix, block.dump());
}

//! Before: `Game Over: competitive  de_dust2 score 9:6 after 38 min`, blocks for rounds 1..14
//! After:  `### check: missing round_stats for rounds 15 (of 15)`
std::string RoundStatsExtractor::CheckAllRoundsSeen(std::string_view gameOverLine)
{
	const auto scoreFirst{ NumberAfter(gameOverLine, " score ") };
	// The second score follows the first one after a colon, `9:6`.
	const auto separator{ gameOverLine.find(':', gameOverLine.find(" score ")) };
	const auto scoreSecond{ separator != std::string_view::npos
								? NumberAfter(gameOverLine.substr(separator), ":")
								: std::nullopt };

	std::string note;
	if (scoreFirst && scoreSecond)
	{
		// A `round_stats` block is logged when each round starts, so every round has one.
		const int totalRounds{ *scoreFirst + *scoreSecond };
		std::string missing;
		for (int round{ 1 }; round <= totalRounds; ++round)
		{
			if (!m_seenRounds.contains(round))
			{
				missing += (missing.empty() ? "" : ", ") + std::to_string(round);
			}
		}

		note = missing.empty()
				   ? std::format("### check: round_stats found for all {} rounds", totalRounds)
				   : std::format("### check: missing round_stats for rounds {} (of {})",
						 missing,
						 totalRounds);
	}
	else
	{
		note = "### check: cannot read the score of the Game Over line";
	}

	m_seenRounds.clear();
	return note;
}
