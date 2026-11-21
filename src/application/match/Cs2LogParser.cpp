#include "Cs2LogParser.h"

#include <algorithm>
#include <charconv>
#include <ranges>
#include <regex>

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

//! `from_chars` ignores the locale, so a decimal comma in the system locale does not matter.
template<typename T>
std::optional<T> ParseNumber(std::string_view text)
{
	T value{};
	const auto [end, error]{ std::from_chars(text.data(), text.data() + text.size(), value) };
	if (error != std::errc{} || end != text.data() + text.size())
	{
		return std::nullopt;
	}

	return value;
}

int ToInt(std::string_view text)
{
	return ParseNumber<int>(text).value_or(0);
}

double ToDouble(std::string_view text)
{
	return ParseNumber<double>(text).value_or(0.0);
}

//! Removes the timestamp prefix. File logs and HTTP logs format it differently:
//! Before: `L 11/21/2026 - 14:03:22: World triggered "Round_Start"`
//! Before: `11/21/2026 - 14:03:22.123 - World triggered "Round_Start"`
//! After:  `World triggered "Round_Start"`
std::string_view StripTimestamp(std::string_view line)
{
	std::string_view rest{ line };
	if (rest.starts_with("L "))
	{
		rest.remove_prefix(2);
	}

	// `MM/DD/YYYY - HH:MM:SS` has a fixed length. Checked by hand, because this runs for every
	// line and a regex is too slow for that.
	constexpr std::size_t dateTimeLength{ 21 };
	if (rest.size() < dateTimeLength || rest[2] != '/' || rest[5] != '/'
		|| rest.substr(10, 3) != " - " || rest[15] != ':' || rest[18] != ':')
	{
		return line;
	}
	rest.remove_prefix(dateTimeLength);

	if (rest.starts_with('.'))
	{
		rest.remove_prefix(std::min(rest.find_first_not_of("0123456789", 1), rest.size()));
	}

	for (const std::string_view separator : { ": ", " - " })
	{
		if (rest.starts_with(separator))
		{
			return rest.substr(separator.size());
		}
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

//! Real players are tagged with their Steam id, bots with `<BOT>`, so bots are skipped.
//! Before: `"Slava<2><[U:1:123456]><CT>" killed "BOT Wesley<3><BOT><TERRORIST>" with "ak47"`
//! After:  PlayerSeen{ 123456, "Slava" }
void AppendPlayersSeen(std::string_view message, std::vector<MatchEvent>& events)
{
	// Cheap check first, most lines have no real player in them.
	if (message.find("[U:1:") == std::string_view::npos)
	{
		return;
	}

	static const std::regex playerTag{ R"re("([^"]*)<\d+><\[U:1:(\d+)\]>)re" };
	using Iterator = std::regex_iterator<std::string_view::const_iterator>;
	for (Iterator it{ message.begin(), message.end(), playerTag }; it != Iterator{}; ++it)
	{
		const auto accountId{ ParseNumber<std::uint32_t>((*it)[2].str()) };
		if (accountId && *accountId != 0)
		{
			events.push_back(PlayerSeen{ *accountId, (*it)[1].str() });
		}
	}
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
			// Without a valid id the player cannot be stored, and must not look like a bot.
			const auto accountId{ ParseNumber<std::uint32_t>(value) };
			if (!accountId)
			{
				return std::nullopt;
			}
			player.m_accountId = *accountId;
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

void Cs2MatchLogParser::Reset()
{
	m_pendingLine.clear();
	m_isInsideJson = false;
	m_jsonValues.clear();
}

std::vector<MatchEvent> Cs2MatchLogParser::Feed(std::string_view chunk)
{
	m_pendingLine += chunk;

	std::vector<MatchEvent> events;
	std::size_t lineStart{ 0 };
	for (auto lineEnd{ m_pendingLine.find('\n') }; lineEnd != std::string::npos;
		lineEnd = m_pendingLine.find('\n', lineStart))
	{
		const std::string_view line{ m_pendingLine.data() + lineStart, lineEnd - lineStart };
		ParseLine(line, events);
		lineStart = lineEnd + 1;
	}
	m_pendingLine.erase(0, lineStart);

	return events;
}

void Cs2MatchLogParser::ParseLine(std::string_view line, std::vector<MatchEvent>& events)
{
	const std::string_view message{ Trim(StripTimestamp(line)) };

	if (message.starts_with("JSON_BEGIN"))
	{
		m_isInsideJson = true;
		m_jsonValues.clear();
		return;
	}

	if (m_isInsideJson)
	{
		if (message.ends_with("JSON_END"))
		{
			m_isInsideJson = false;
			if (auto stats{ FinishJsonBlock() })
			{
				events.push_back(std::move(*stats));
			}
			return;
		}

		// Before: `"player_0" : "  123456,      3, ...",`
		static const std::regex keyValue{ R"re(^"([^"]+)"\s*:\s*"([^"]*)"\s*,?$)re" };
		std::match_results<std::string_view::const_iterator> match;
		if (std::regex_match(message.begin(), message.end(), match, keyValue))
		{
			m_jsonValues.insert_or_assign(match.str(1), match.str(2));
		}
		return;
	}

	// Before: `World triggered "Match_Start" on "de_dust2"`
	constexpr std::string_view matchStart{ R"(World triggered "Match_Start" on ")" };
	if (message.starts_with(matchStart))
	{
		const auto mapName{ message.substr(matchStart.size()) };
		events.push_back(MatchStarted{ ToMapInfo(mapName.substr(0, mapName.find('"'))) });
		return;
	}

	if (message.starts_with("Game Over:"))
	{
		events.push_back(MatchEnded{});
		return;
	}

	AppendPlayersSeen(message, events);
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
