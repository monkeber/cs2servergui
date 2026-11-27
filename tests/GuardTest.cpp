#include "gtest/gtest.h"

#include <match/Guard.h>

#include <stdexcept>

TEST(GuardTest, ReturnsTrueWhenTheStepSucceeds)
{
	bool hasReported{ false };
	bool ran{ false };

	EXPECT_TRUE(RunGuarded("step", hasReported, [&] {
		ran = true;
	}));

	EXPECT_TRUE(ran);
	EXPECT_FALSE(hasReported);
}

TEST(GuardTest, SwallowsAStdException)
{
	bool hasReported{ false };

	bool result{ true };
	ASSERT_NO_THROW(result = RunGuarded("step", hasReported, [] {
		throw std::runtime_error{ "disk is full" };
	}));

	EXPECT_FALSE(result);
	EXPECT_TRUE(hasReported);
}

TEST(GuardTest, SwallowsAnythingElseThatIsThrown)
{
	bool hasReported{ false };

	bool result{ true };
	ASSERT_NO_THROW(result = RunGuarded("step", hasReported, [] {
		throw 42;
	}));

	EXPECT_FALSE(result);
	EXPECT_TRUE(hasReported);
}

TEST(GuardTest, ASuccessEndsTheStreakOfFailures)
{
	bool hasReported{ false };

	RunGuarded("step", hasReported, [] {
		throw std::runtime_error{ "x" };
	});
	ASSERT_TRUE(hasReported);
	RunGuarded("step", hasReported, [] {});

	EXPECT_FALSE(hasReported);
}
