#include "gtest/gtest.h"

#include <match/LogParser.h>

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

TEST(MatchLogParserTest, ParsesSampleMatch)
{
	MatchLogParser parser;
	const auto events{ parser.Feed(ReadFixture("sample_match.log")) };

	ASSERT_EQ(events.size(), 4);
	ASSERT_TRUE(std::holds_alternative<MatchStarted>(events[0]));
	ASSERT_EQ(std::get<MatchStarted>(events[0]).m_mapName, "de_dust2");
	ASSERT_TRUE(std::holds_alternative<RoundStats>(events[1]));
	ASSERT_TRUE(std::holds_alternative<RoundStats>(events[2]));
	ASSERT_TRUE(std::holds_alternative<MatchEnded>(events[3]));

	const auto& round{ std::get<RoundStats>(events[2]) };
	ASSERT_EQ(round.m_mapName, "de_dust2");
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

TEST(MatchLogParserTest, HandlesChunksSplitAnywhere)
{
	const std::string log{ ReadFixture("sample_match.log") };

	MatchLogParser parser;
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

TEST(MatchLogParserTest, ParsesFileLogFormat)
{
	MatchLogParser parser;
	const auto events{ parser.Feed(
		"L 11/21/2026 - 14:00:05: World triggered \"Match_Start\" on \"de_mirage\"\n") };

	ASSERT_EQ(events.size(), 1);
	ASSERT_EQ(std::get<MatchStarted>(events[0]).m_mapName, "de_mirage");
}

TEST(MatchLogParserTest, IgnoresOtherJsonBlocks)
{
	MatchLogParser parser;
	const auto events{ parser.Feed(
		"JSON_BEGIN{\n"
		"\"name\": \"something_else\",\n"
		"}}JSON_END\n") };

	ASSERT_TRUE(events.empty());
}
