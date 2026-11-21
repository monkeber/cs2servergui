#include "gtest/gtest.h"

#include <MatchStatsService.h>

#include <fstream>
#include <sstream>

TEST(MatchStatsServiceTest, ReplayOfSampleMatchStoresFinalTotals)
{
	std::ifstream file{ std::string{ FIXTURES_DIR } + "/sample_match.log" };
	std::stringstream log;
	log << file.rdbuf();

	MatchStatsDatabase db{ ":memory:" };
	MatchStatsService service{ db };
	service.HandleLog(log.str());

	const auto matches{ db.SelectMatches() };
	ASSERT_EQ(matches.size(), 1);
	ASSERT_EQ(matches.front().m_mapName, "de_dust2");
	ASSERT_EQ(matches.front().m_scoreCt, 1);
	ASSERT_EQ(matches.front().m_scoreT, 1);
	ASSERT_TRUE(matches.front().m_endedAt.has_value());

	const auto players{ db.SelectPlayerStats(matches.front().m_id) };
	ASSERT_EQ(players.size(), 2);
	ASSERT_EQ(players[0].m_accountId, 123456);
	ASSERT_EQ(players[0].m_kills, 2);
	ASSERT_EQ(players[1].m_accountId, 654321);
	ASSERT_DOUBLE_EQ(players[1].m_adr, 72.5);
}

TEST(MatchStatsServiceTest, RoundWithoutMatchStartCreatesMatch)
{
	MatchStatsDatabase db{ ":memory:" };
	MatchStatsService service{ db };
	service.HandleLog(
		"JSON_BEGIN{\n"
		"\"name\": \"round_stats\",\n"
		"\"map\" : \"de_inferno\",\n"
		"\"fields\" : \"accountid, kills\",\n"
		"\"player_0\" : \"42, 7\"\n"
		"}}JSON_END\n");

	const auto matches{ db.SelectMatches() };
	ASSERT_EQ(matches.size(), 1);
	ASSERT_EQ(matches.front().m_mapName, "de_inferno");
	ASSERT_EQ(db.SelectPlayerStats(matches.front().m_id).front().m_kills, 7);
}
