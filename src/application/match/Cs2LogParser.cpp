#include "Cs2LogParser.h"

#include <charconv>
#include <locale>
#include <ranges>
#include <regex>
#include <sstream>

namespace
{

std::string_view Trim(std::string_view text)
{
	const auto first{ text.find_first_not_of(" \t\r") };
	if (first == std::string_view::npos)
	{
		return {};
	}
	const auto last{ text.find_last_not_of(" \t\r") };

	return text.substr(first, last - first + 1);
}

std::vector<std::string_view> SplitCsv(std::string_view text)
{
	std::vector<std::string_view> result;
	for (const auto part : std::views::split(text, ','))
	{
		result.push_back(Trim(std::string_view{ part }));
	}

	return result;
}

int ToInt(std::string_view text)
{
	int value{ 0 };
	std::from_chars(text.data(), text.data() + text.size(), value);

	return value;
}

double ToDouble(std::string_view text)
{
	// Classic locale, because Qt applies the system locale, where the decimal separator may be a
	// comma.
	std::istringstream stream{ std::string{ text } };
	stream.imbue(std::locale::classic());
	double value{ 0.0 };
	stream >> value;

	return value;
}

//! Removes the timestamp prefix. File logs and HTTP logs format it differently:
//! Before: `L 11/21/2026 - 14:03:22: World triggered "Round_Start"`
//! Before: `11/21/2026 - 14:03:22.123 - World triggered "Round_Start"`
//! After:  `World triggered "Round_Start"`
std::string_view StripTimestamp(std::string_view line)
{
	static const std::regex prefix{
		R"re(^(?:L )?\d{2}/\d{2}/\d{4} - \d{2}:\d{2}:\d{2}(?:\.\d+)?(?::| -) )re"
	};

	std::match_results<std::string_view::const_iterator> match;
	if (std::regex_search(line.begin(), line.end(), match, prefix))
	{
		return line.substr(match.length(0));
	}

	return line;
}

//! Workshop maps are logged with their workshop path.
//! Before: `workshop/3070284539/de_example`
//! After:  { name: `workshop/3070284539/de_example`, workshopId: `3070284539` }
MapInfo ToMapInfo(std::string_view mapName)
{
	static const std::regex workshopPath{ R"re(workshop/(\d+)/)re" };

	MapInfo map{ .m_name = std::string{ mapName } };
	std::smatch match;
	if (std::regex_search(map.m_name, match, workshopPath))
	{
		map.m_workshopId = match.str(1);
	}

	return map;
}

std::optional<PlayerStats> ToPlayerStats(
	const std::vector<std::string_view>& columns, const std::vector<std::string_view>& values)
{
	if (columns.size() != values.size())
	{
		return std::nullopt;
	}

	PlayerStats player;
	for (const auto [column, value] : std::views::zip(columns, values))
	{
		if (column == "accountid")
		{
			player.m_accountId = static_cast<std::uint32_t>(ToInt(value));
		}
		else if (column == "team")
		{
			player.m_team = ToInt(value);
		}
		else if (column == "kills")
		{
			player.m_kills = ToInt(value);
		}
		else if (column == "deaths")
		{
			player.m_deaths = ToInt(value);
		}
		else if (column == "assists")
		{
			player.m_assists = ToInt(value);
		}
		else if (column == "dmg")
		{
			player.m_damage = ToInt(value);
		}
		else if (column == "hsp")
		{
			player.m_headshotPercent = ToDouble(value);
		}
		else if (column == "adr")
		{
			player.m_adr = ToDouble(value);
		}
		else if (column == "mvp")
		{
			player.m_mvp = ToInt(value);
		}
	}

	return player;
}

}	 // namespace

std::vector<MatchEvent> Cs2MatchLogParser::Feed(std::string_view chunk)
{
	m_pendingLine += chunk;

	std::vector<MatchEvent> events;
	std::size_t lineStart{ 0 };
	for (auto lineEnd{ m_pendingLine.find('\n') }; lineEnd != std::string::npos;
		lineEnd = m_pendingLine.find('\n', lineStart))
	{
		const std::string_view line{ m_pendingLine.data() + lineStart, lineEnd - lineStart };
		if (auto event{ ParseLine(line) })
		{
			events.push_back(std::move(*event));
		}
		lineStart = lineEnd + 1;
	}
	m_pendingLine.erase(0, lineStart);

	return events;
}

std::optional<MatchEvent> Cs2MatchLogParser::ParseLine(std::string_view line)
{
	const std::string_view message{ Trim(StripTimestamp(line)) };

	if (message.starts_with("JSON_BEGIN"))
	{
		m_isInsideJson = true;
		m_jsonValues.clear();
		return std::nullopt;
	}

	if (m_isInsideJson)
	{
		if (message.ends_with("JSON_END"))
		{
			m_isInsideJson = false;
			return FinishJsonBlock();
		}

		// Before: `"player_0" : "  123456,      3, ...",`
		static const std::regex keyValue{ R"re(^"([^"]+)"\s*:\s*"([^"]*)"\s*,?$)re" };
		std::match_results<std::string_view::const_iterator> match;
		if (std::regex_match(message.begin(), message.end(), match, keyValue))
		{
			m_jsonValues.insert_or_assign(match.str(1), match.str(2));
		}
		return std::nullopt;
	}

	static const std::regex matchStart{ R"re(^World triggered "Match_Start" on "([^"]+)")re" };
	std::match_results<std::string_view::const_iterator> match;
	if (std::regex_match(message.begin(), message.end(), match, matchStart))
	{
		return MatchStarted{ ToMapInfo(match.str(1)) };
	}

	if (message.starts_with("Game Over:"))
	{
		return MatchEnded{};
	}

	return std::nullopt;
}

std::optional<MatchEvent> Cs2MatchLogParser::FinishJsonBlock()
{
	const auto value{ [this](std::string_view key) -> std::string_view {
		const auto it{ m_jsonValues.find(key) };
		return it != m_jsonValues.end() ? std::string_view{ it->second } : std::string_view{};
	} };

	if (value("name") != "round_stats")
	{
		return std::nullopt;
	}

	RoundStats stats;
	stats.m_map = ToMapInfo(value("map"));
	stats.m_roundNumber = ToInt(value("round_number"));
	stats.m_scoreCt = ToInt(value("score_ct"));
	stats.m_scoreT = ToInt(value("score_t"));

	// Columns are matched by name, so a change of the field order in a game update is harmless.
	const auto columns{ SplitCsv(value("fields")) };
	for (const auto& [key, playerValues] : m_jsonValues)
	{
		if (!key.starts_with("player_"))
		{
			continue;
		}
		if (auto player{ ToPlayerStats(columns, SplitCsv(playerValues)) })
		{
			stats.m_players.push_back(*player);
		}
	}

	return stats;
}
