#include "gtest/gtest.h"

#include <match/StatsDatabase.h>

namespace
{

RoundStats MakeRound(int kills)
{
	return RoundStats{
		.m_mapName = "de_dust2",
		.m_roundNumber = 1,
		.m_scoreCt = 1,
		.m_scoreT = 0,
		.m_players = {
			PlayerStats{ .m_accountId = 123456, .m_team = 3, .m_kills = kills },
			PlayerStats{ .m_accountId = 0, .m_team = 2, .m_kills = 5 },
		},
	};
}

}	 // namespace

TEST(MatchStatsDatabaseTest, SaveRoundReplacesTotalsAndSkipsBots)
{
	MatchStatsDatabase db{ ":memory:" };
	const auto matchId{ db.CreateMatch("de_dust2") };

	db.SaveRound(matchId, MakeRound(1));
	db.SaveRound(matchId, MakeRound(3));

	const auto players{ db.SelectPlayerStats(matchId) };
	ASSERT_EQ(players.size(), 1);
	ASSERT_EQ(players.front().m_accountId, 123456);
	ASSERT_EQ(players.front().m_kills, 3);

	const auto matches{ db.SelectMatches() };
	ASSERT_EQ(matches.size(), 1);
	ASSERT_EQ(matches.front().m_scoreCt, 1);
	ASSERT_FALSE(matches.front().m_endedAt.has_value());
}

TEST(MatchStatsDatabaseTest, EndMatchSetsEndTime)
{
	MatchStatsDatabase db{ ":memory:" };
	const auto matchId{ db.CreateMatch("de_dust2") };

	db.EndMatch(matchId);

	ASSERT_TRUE(db.SelectMatches().front().m_endedAt.has_value());
}

TEST(MatchStatsDatabaseTest, CreateMatchExtractsWorkshopId)
{
	MatchStatsDatabase db{ ":memory:" };
	db.CreateMatch("workshop/3070284539/de_example");
	db.CreateMatch("de_dust2");

	const auto matches{ db.SelectMatches() };
	ASSERT_EQ(matches[0].m_workshopId, "3070284539");
	ASSERT_FALSE(matches[1].m_workshopId.has_value());
}
