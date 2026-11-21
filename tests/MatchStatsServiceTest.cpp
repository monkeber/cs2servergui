#include "gtest/gtest.h"

#include <match/Cs2LogParser.h>
#include <match/StatsService.h>

#include <format>
#include <fstream>
#include <map>
#include <sstream>

namespace
{

//! Storage is an I/O boundary, so the service is tested against an in-memory fake.
class FakeStore final : public MatchStatsStore
{
public:
	struct Match
	{
		MapInfo m_map;
		std::optional<RoundStats> m_lastRound;
		bool m_isEnded{ false };
	};

	std::int64_t CreateMatch(const MapInfo& map) override
	{
		m_matches.push_back(Match{ .m_map = map });
		return static_cast<std::int64_t>(m_matches.size() - 1);
	}

	void SaveRound(std::int64_t matchId, const RoundStats& stats) override
	{
		m_matches.at(matchId).m_lastRound = stats;
	}

	void EndMatch(std::int64_t matchId) override
	{
		m_matches.at(matchId).m_isEnded = true;
	}

	std::vector<Match> m_matches;
};

std::string RoundStatsLog(const std::string& map, int round, int kills)
{
	return std::format(
		"JSON_BEGIN{{\n"
		"\"name\": \"round_stats\",\n"
		"\"round_number\" : \"{}\",\n"
		"\"map\" : \"{}\",\n"
		"\"fields\" : \"accountid, kills\",\n"
		"\"player_0\" : \"42, {}\"\n"
		"}}}}JSON_END\n",
		round,
		map,
		kills);
}

}	 // namespace

TEST(MatchStatsServiceTest, ReplayOfSampleMatchStoresFinalTotalsOfRealPlayers)
{
	std::ifstream file{ std::string{ FIXTURES_DIR } + "/sample_match.log" };
	std::stringstream log;
	log << file.rdbuf();

	Cs2MatchLogParser parser;
	FakeStore store;
	MatchStatsService service{ parser, store };
	service.HandleLog(log.str());

	ASSERT_EQ(store.m_matches.size(), 1);
	const auto& match{ store.m_matches.front() };
	ASSERT_EQ(match.m_map.m_name, "de_dust2");
	ASSERT_TRUE(match.m_isEnded);
	ASSERT_EQ(match.m_lastRound->m_roundNumber, 2);

	const auto& players{ match.m_lastRound->m_players };
	ASSERT_EQ(players.size(), 2);
	ASSERT_EQ(players[0].m_accountId, 123456);
	ASSERT_EQ(players[0].m_kills, 2);
	ASSERT_EQ(players[1].m_accountId, 654321);
}

TEST(MatchStatsServiceTest, RoundWithoutMatchStartCreatesMatch)
{
	Cs2MatchLogParser parser;
	FakeStore store;
	MatchStatsService service{ parser, store };
	service.HandleLog(
		"JSON_BEGIN{\n"
		"\"name\": \"round_stats\",\n"
		"\"map\" : \"de_inferno\",\n"
		"\"fields\" : \"accountid, kills\",\n"
		"\"player_0\" : \"42, 7\"\n"
		"}}JSON_END\n");

	ASSERT_EQ(store.m_matches.size(), 1);
	ASSERT_EQ(store.m_matches.front().m_map.m_name, "de_inferno");
	ASSERT_EQ(store.m_matches.front().m_lastRound->m_players.front().m_kills, 7);
	ASSERT_FALSE(store.m_matches.front().m_isEnded);
}

TEST(MatchStatsServiceTest, MapChangeWithoutMatchStartStartsNewMatch)
{
	Cs2MatchLogParser parser;
	FakeStore store;
	MatchStatsService service{ parser, store };
	service.HandleLog(RoundStatsLog("de_dust2", 1, 3));
	service.HandleLog(RoundStatsLog("de_mirage", 1, 1));

	ASSERT_EQ(store.m_matches.size(), 2);
	ASSERT_EQ(store.m_matches[0].m_lastRound->m_players.front().m_kills, 3);
	ASSERT_FALSE(store.m_matches[0].m_isEnded);
	ASSERT_EQ(store.m_matches[1].m_map.m_name, "de_mirage");
}

TEST(MatchStatsServiceTest, RoundNumberGoingBackStartsNewMatch)
{
	Cs2MatchLogParser parser;
	FakeStore store;
	MatchStatsService service{ parser, store };
	service.HandleLog(RoundStatsLog("de_dust2", 1, 1));
	service.HandleLog(RoundStatsLog("de_dust2", 2, 2));
	service.HandleLog(RoundStatsLog("de_dust2", 1, 0));

	ASSERT_EQ(store.m_matches.size(), 2);
	ASSERT_EQ(store.m_matches[0].m_lastRound->m_roundNumber, 2);
}

TEST(MatchStatsServiceTest, MatchStartWithoutRoundsStoresNothing)
{
	Cs2MatchLogParser parser;
	FakeStore store;
	MatchStatsService service{ parser, store };
	service.HandleLog("World triggered \"Match_Start\" on \"de_dust2\"\n");
	service.HandleLog("World triggered \"Match_Start\" on \"de_dust2\"\n");

	ASSERT_TRUE(store.m_matches.empty());
}

TEST(MatchStatsServiceTest, ResetStartsNewMatchOnSameMap)
{
	Cs2MatchLogParser parser;
	FakeStore store;
	MatchStatsService service{ parser, store };
	service.HandleLog(RoundStatsLog("de_dust2", 1, 1));
	service.Reset();
	service.HandleLog(RoundStatsLog("de_dust2", 2, 5));

	ASSERT_EQ(store.m_matches.size(), 2);
	ASSERT_EQ(store.m_matches[0].m_lastRound->m_players.front().m_kills, 1);
}
