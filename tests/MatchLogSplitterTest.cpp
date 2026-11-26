#include "gtest/gtest.h"

#include <match/MatchLogSplitter.h>

#include <format>

namespace
{

constexpr auto kMatchStart{ "t - World triggered \"Match_Start\" on \"de_dust2\"\n" };
constexpr auto kRoundStart{ "t - World triggered \"Round_Start\"\n" };

std::string RoundBlock(int round)
{
	return std::format("t - JSON_BEGIN{{\nt - \"name\": \"round_stats\",\n"
					   "t - \"round_number\" : \"{}\",\nt - }}}}JSON_END\n",
		round);
}

std::string Joined(const std::vector<MatchLogSplitter::Piece>& pieces)
{
	std::string text;
	for (const auto& piece : pieces)
	{
		text += piece.m_text;
	}
	return text;
}

}	 // namespace

TEST(MatchLogSplitterTest, KeepsOneChunkTogetherWithoutNewMatch)
{
	MatchLogSplitter splitter;
	const std::string chunk{ RoundBlock(2) + "t - other\n" };

	const auto pieces{ splitter.Split(chunk) };

	ASSERT_EQ(pieces.size(), 1u);
	EXPECT_EQ(pieces[0].m_text, chunk);
	EXPECT_FALSE(pieces[0].m_startsNewMatch);
}

TEST(MatchLogSplitterTest, IgnoresFirstRoundBlockBeforeAnyRoundWasPlayed)
{
	MatchLogSplitter splitter;

	// Warmup restart: the block and `Match_Start` are logged again.
	const auto pieces{ splitter.Split(RoundBlock(1) + kMatchStart + RoundBlock(1) + kMatchStart) };

	ASSERT_EQ(pieces.size(), 1u);
	EXPECT_FALSE(pieces[0].m_startsNewMatch);
}

TEST(MatchLogSplitterTest, NewMatchStartsWithItsFirstRoundBlock)
{
	MatchLogSplitter splitter;
	const std::string previous{ std::string{ kRoundStart } + "t - Game Over: x\n" };
	const std::string next{ RoundBlock(1) + kMatchStart + "t - Warmup_Start\n" };

	const auto pieces{ splitter.Split(previous + next) };

	// The block comes before `Match_Start` in the log, it belongs to the new match.
	ASSERT_EQ(pieces.size(), 2u);
	EXPECT_EQ(pieces[0].m_text, previous);
	EXPECT_FALSE(pieces[0].m_startsNewMatch);
	EXPECT_EQ(pieces[1].m_text, next);
	EXPECT_TRUE(pieces[1].m_startsNewMatch);
}

TEST(MatchLogSplitterTest, OtherRoundBlocksAreNoNewMatch)
{
	MatchLogSplitter splitter;
	splitter.Split(kRoundStart);

	for (const int round : { 2, 10, 11, 21 })
	{
		const auto pieces{ splitter.Split(RoundBlock(round)) };
		ASSERT_EQ(pieces.size(), 1u) << round;
		EXPECT_FALSE(pieces[0].m_startsNewMatch) << round;
	}
}

TEST(MatchLogSplitterTest, HoldsBackBlockThatIsNotCompleteYet)
{
	MatchLogSplitter splitter;
	splitter.Split(kRoundStart);
	const std::string block{ RoundBlock(1) };
	const auto split{ block.find("round_number") };

	// The chunk ends before the round number is known.
	const auto first{ splitter.Split("t - Game Over: x\n" + block.substr(0, split)) };
	const auto second{ splitter.Split(block.substr(split) + kMatchStart) };

	ASSERT_EQ(first.size(), 1u);
	EXPECT_EQ(first[0].m_text, "t - Game Over: x\n");
	EXPECT_FALSE(first[0].m_startsNewMatch);
	ASSERT_EQ(second.size(), 1u);
	EXPECT_EQ(second[0].m_text, block + kMatchStart);
	EXPECT_TRUE(second[0].m_startsNewMatch);
}

TEST(MatchLogSplitterTest, KeepsNewMatchFlagWhenBlockIsHeldBackAfterTheCut)
{
	MatchLogSplitter splitter;
	splitter.Split(kRoundStart);
	const std::string block{ RoundBlock(1) };

	// Round number seen, so the cut is made, but the block is not finished.
	const auto first{ splitter.Split("t - Game Over: x\n" + block.substr(0, block.find("}}JSON_END"))) };
	const auto second{ splitter.Split(block.substr(block.find("}}JSON_END"))) };

	ASSERT_EQ(first.size(), 1u);
	EXPECT_EQ(first[0].m_text, "t - Game Over: x\n");
	ASSERT_EQ(second.size(), 1u);
	EXPECT_EQ(second[0].m_text, block);
	EXPECT_TRUE(second[0].m_startsNewMatch);
}

TEST(MatchLogSplitterTest, NeverLosesOrReordersText)
{
	MatchLogSplitter splitter;
	const std::string log{ std::string{ kRoundStart } + RoundBlock(2) + "t - Game Over: x\n"
						   + RoundBlock(1) + kMatchStart + kRoundStart + RoundBlock(2) };

	std::string joined;
	// Every possible chunk boundary.
	for (std::size_t cut{ 0 }; cut <= log.size(); ++cut)
	{
		MatchLogSplitter fresh;
		joined = Joined(fresh.Split(log.substr(0, cut)));
		joined += Joined(fresh.Split(log.substr(cut)));
		joined += Joined(fresh.Flush());
		ASSERT_EQ(joined, log) << cut;
	}
}

TEST(MatchLogSplitterTest, FlushReturnsHeldBackBlock)
{
	MatchLogSplitter splitter;
	splitter.Split("t - JSON_BEGIN{\n");

	const auto pieces{ splitter.Flush() };

	ASSERT_EQ(pieces.size(), 1u);
	EXPECT_EQ(pieces[0].m_text, "t - JSON_BEGIN{\n");
	EXPECT_TRUE(splitter.Flush().empty());
}

TEST(MatchLogSplitterTest, ServerRestartMakesNextMatchNew)
{
	MatchLogSplitter splitter;
	splitter.Reset();

	const auto pieces{ splitter.Split(RoundBlock(1) + kMatchStart) };

	ASSERT_EQ(pieces.size(), 1u);
	EXPECT_TRUE(pieces[0].m_startsNewMatch);
}
