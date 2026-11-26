#include "gtest/gtest.h"

#include <match/RoundStatsExtractor.h>

#include <format>

namespace
{

std::string RoundBlock(int round)
{
	return std::format("t - JSON_BEGIN{{\nt - \"name\": \"round_stats\",\n"
					   "t - \"round_number\" : \"{}\",\nt - }}}}JSON_END\n",
		round);
}

constexpr auto kGameOver{ "t - Game Over: competitive  de_dust2 score 2:1 after 9 min\r\n" };

}	 // namespace

TEST(RoundStatsExtractorTest, WritesBlockAsOneLine)
{
	RoundStatsExtractor extractor;

	std::vector<std::string> lines;
	for (const auto& chunk : std::vector<std::string>{
			 "t - \"A<1><BOT><CT>\" purchased \"ak47\"\n",
			 "t - JSON_BEGIN{\nt - \"name\": \"round_stats\",\nt - \"round_number\" : \"1\",\n"
			 "t - \"score_t\" : \"0\",\nt - \"map\" : \"de_dust2\",\n"
			 "t - \"fields\" : \"             accountid,   team,    adr,    hsp\",\n"
			 "t - \"players\" : {\n",
			 // A line split between two chunks must be put back together.
			 "t - \"player_2\" : \"                   0,      3,     63,  16.67",
			 "\"\nt - \"player_10\" : \"                 0,      2,     87,   0.00\"\n"
			 "t - }}JSON_END\n",
			 "t - \"A<1><BOT><CT>\" killed \"B<2><BOT><T>\"\n",
		 })
	{
		for (auto& line : extractor.Feed(chunk))
		{
			lines.push_back(std::move(line));
		}
	}

	// The whole block is one line, numbers are numbers.
	ASSERT_EQ(lines.size(), 1u);
	EXPECT_EQ(lines[0],
		"t - ROUND_STATS {\"name\":\"round_stats\",\"round_number\":1,\"score_t\":0,"
		"\"map\":\"de_dust2\",\"players\":["
		"{\"userid\":2,\"accountid\":0,\"team\":3,\"adr\":63,\"hsp\":16.67},"
		"{\"userid\":10,\"accountid\":0,\"team\":2,\"adr\":87,\"hsp\":0.0}]}");
}

TEST(RoundStatsExtractorTest, NotesWhenAllRoundsHaveStats)
{
	RoundStatsExtractor extractor;
	for (int round{ 1 }; round <= 3; ++round)
	{
		extractor.Feed(RoundBlock(round));
	}

	const auto lines{ extractor.Feed(kGameOver) };

	ASSERT_EQ(lines.size(), 1u);
	EXPECT_EQ(lines[0], "### check: round_stats found for all 3 rounds");
}

TEST(RoundStatsExtractorTest, NotesMissingRounds)
{
	RoundStatsExtractor extractor;
	extractor.Feed(RoundBlock(1));
	extractor.Feed(RoundBlock(3));

	const auto lines{ extractor.Feed(kGameOver) };

	ASSERT_EQ(lines.size(), 1u);
	EXPECT_EQ(lines[0], "### check: missing round_stats for rounds 2 (of 3)");
}

TEST(RoundStatsExtractorTest, NewMatchForgetsRoundsOfThePreviousOne)
{
	RoundStatsExtractor extractor;
	extractor.Feed(RoundBlock(1));
	extractor.Feed(RoundBlock(2));
	extractor.Feed(RoundBlock(3));
	extractor.StartNewMatch();
	extractor.Feed(RoundBlock(1));

	const auto lines{ extractor.Feed(kGameOver) };

	ASSERT_EQ(lines.size(), 1u);
	EXPECT_EQ(lines[0], "### check: missing round_stats for rounds 2, 3 (of 3)");
}

TEST(RoundStatsExtractorTest, ResetDropsBlockThatWasCutByARestart)
{
	RoundStatsExtractor extractor;
	extractor.Feed("t - JSON_BEGIN{\nt - \"round_number\" : \"7\",\n");
	extractor.Reset();

	const auto lines{ extractor.Feed("t - something else\n") };

	EXPECT_TRUE(lines.empty());
}
