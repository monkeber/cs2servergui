#include "gtest/gtest.h"

#include <match/SqliteStatsStore.h>

namespace
{

RoundStats MakeRound(int kills)
{
	return RoundStats{
		.m_map = { .m_name = "de_dust2" },
		.m_roundNumber = 1,
		.m_scoreCt = 1,
		.m_scoreT = 0,
		.m_players = { PlayerStats{ .m_accountId = 123456, .m_team = 3, .m_kills = kills } },
	};
}

}	 // namespace

TEST(SqliteMatchStatsStoreTest, SaveRoundReplacesPlayerTotals)
{
	SqliteMatchStatsStore store{ ":memory:" };
	const auto matchId{ store.CreateMatch({ .m_name = "de_dust2" }) };

	store.SaveRound(matchId, MakeRound(1));
	store.SaveRound(matchId, MakeRound(3));

	const auto players{ store.SelectPlayerStats(matchId) };
	ASSERT_EQ(players.size(), 1);
	ASSERT_EQ(players.front().m_stats.m_accountId, 123456);
	ASSERT_EQ(players.front().m_stats.m_kills, 3);
	ASSERT_FALSE(players.front().m_name.has_value());

	const auto matches{ store.SelectMatches() };
	ASSERT_EQ(matches.size(), 1);
	ASSERT_EQ(matches.front().m_scoreCt, 1);
	ASSERT_FALSE(matches.front().m_endedAt.has_value());
}

TEST(SqliteMatchStatsStoreTest, EndMatchSetsEndTime)
{
	SqliteMatchStatsStore store{ ":memory:" };
	const auto matchId{ store.CreateMatch({ .m_name = "de_dust2" }) };

	store.EndMatch(matchId);

	ASSERT_TRUE(store.SelectMatches().front().m_endedAt.has_value());
}

TEST(SqliteMatchStatsStoreTest, StoresWorkshopId)
{
	SqliteMatchStatsStore store{ ":memory:" };
	store.CreateMatch({ .m_name = "workshop/3070284539/de_example", .m_workshopId = "3070284539" });
	store.CreateMatch({ .m_name = "de_dust2" });

	const auto matches{ store.SelectMatches() };
	ASSERT_EQ(matches[0].m_map.m_workshopId, "3070284539");
	ASSERT_FALSE(matches[1].m_map.m_workshopId.has_value());
}

TEST(SqliteMatchStatsStoreTest, ScoreboardShowsLatestPlayerName)
{
	SqliteMatchStatsStore store{ ":memory:" };
	const auto matchId{ store.CreateMatch({ .m_name = "de_dust2" }) };
	store.SaveRound(matchId, MakeRound(2));

	store.SavePlayerName(123456, "OldName");
	store.SavePlayerName(123456, "Slava");

	const auto players{ store.SelectPlayerStats(matchId) };
	ASSERT_EQ(players.front().m_name, "Slava");
}
