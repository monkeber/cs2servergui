#include "gtest/gtest.h"

#include <match/Cs2LogParser.h>

#include <fstream>
#include <sstream>

namespace
{

std::string ReadFixture(const std::string& name)
{
	std::ifstream file{ std::string{ FIXTURES_DIR } + "/" + name };
	std::stringstream content;
	content << file.rdbuf();

	return content.str();
}

}	 // namespace

TEST(Cs2MatchLogParserTest, ParsesSampleMatch)
{
	Cs2MatchLogParser parser;
	const auto events{ parser.Feed(ReadFixture("sample_match.log")) };

	ASSERT_EQ(events.size(), 4);
	ASSERT_TRUE(std::holds_alternative<MatchStarted>(events[0]));
	ASSERT_EQ(std::get<MatchStarted>(events[0]).m_map.m_name, "de_dust2");
	ASSERT_TRUE(std::holds_alternative<RoundStats>(events[1]));
	ASSERT_TRUE(std::holds_alternative<RoundStats>(events[2]));
	ASSERT_TRUE(std::holds_alternative<MatchEnded>(events[3]));

	const auto& round{ std::get<RoundStats>(events[2]) };
	ASSERT_EQ(round.m_map.m_name, "de_dust2");
	ASSERT_FALSE(round.m_map.m_workshopId.has_value());
	ASSERT_EQ(round.m_roundNumber, 2);
	ASSERT_EQ(round.m_scoreCt, 1);
	ASSERT_EQ(round.m_scoreT, 1);
	ASSERT_EQ(round.m_players.size(), 3);

	const auto& player{ round.m_players[0] };
	ASSERT_EQ(player.m_accountId, 123456);
	ASSERT_EQ(player.m_team, 3);
	ASSERT_EQ(player.m_kills, 2);
	ASSERT_EQ(player.m_deaths, 1);
	ASSERT_EQ(player.m_assists, 1);
	ASSERT_EQ(player.m_damage, 250);
	ASSERT_DOUBLE_EQ(player.m_headshotPercent, 50.0);
	ASSERT_EQ(player.m_mvp, 1);
	ASSERT_DOUBLE_EQ(round.m_players[2].m_adr, 72.5);
}

TEST(Cs2MatchLogParserTest, HandlesChunksSplitAnywhere)
{
	const std::string log{ ReadFixture("sample_match.log") };

	Cs2MatchLogParser parser;
	std::vector<MatchEvent> events;
	constexpr std::size_t chunkSize{ 7 };
	for (std::size_t pos{ 0 }; pos < log.size(); pos += chunkSize)
	{
		for (auto& event : parser.Feed(std::string_view{ log }.substr(pos, chunkSize)))
		{
			events.push_back(std::move(event));
		}
	}

	ASSERT_EQ(events.size(), 4);
	ASSERT_EQ(std::get<RoundStats>(events[2]).m_players.size(), 3);
}

TEST(Cs2MatchLogParserTest, ParsesFileLogFormatAndWorkshopMap)
{
	Cs2MatchLogParser parser;
	const auto events{ parser.Feed(
		"L 11/21/2026 - 14:00:05: World triggered \"Match_Start\" on "
		"\"workshop/3070284539/de_example\"\n") };

	ASSERT_EQ(events.size(), 1);
	const auto& map{ std::get<MatchStarted>(events[0]).m_map };
	ASSERT_EQ(map.m_name, "workshop/3070284539/de_example");
	ASSERT_EQ(map.m_workshopId, "3070284539");
}

TEST(Cs2MatchLogParserTest, IgnoresOtherJsonBlocks)
{
	Cs2MatchLogParser parser;
	const auto events{ parser.Feed(
		"JSON_BEGIN{\n"
		"\"name\": \"something_else\",\n"
		"}}JSON_END\n") };

	ASSERT_TRUE(events.empty());
}

TEST(Cs2MatchLogParserTest, ParsesAccountIdsAboveSignedIntRange)
{
	Cs2MatchLogParser parser;
	const auto events{ parser.Feed(
		"JSON_BEGIN{\n"
		"\"name\": \"round_stats\",\n"
		"\"fields\" : \"accountid, kills\",\n"
		"\"player_0\" : \"3000000000, 4\",\n"
		"\"player_1\" : \"not_a_number, 9\"\n"
		"}}JSON_END\n") };

	ASSERT_EQ(events.size(), 1);
	const auto& players{ std::get<RoundStats>(events[0]).m_players };
	ASSERT_EQ(players.size(), 1);
	ASSERT_EQ(players.front().m_accountId, 3000000000u);
	ASSERT_EQ(players.front().m_kills, 4);
}

TEST(Cs2MatchLogParserTest, ResetDropsUnfinishedJsonBlock)
{
	Cs2MatchLogParser parser;
	parser.Feed("JSON_BEGIN{\n\"name\": \"round_stats\",\n\"map\" : \"de_du");
	parser.Reset();

	const auto events{ parser.Feed("World triggered \"Match_Start\" on \"de_nuke\"\n") };

	ASSERT_EQ(events.size(), 1);
	ASSERT_EQ(std::get<MatchStarted>(events[0]).m_map.m_name, "de_nuke");
}
