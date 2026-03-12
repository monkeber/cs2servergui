#include <gtest/gtest.h>

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	// If you need to run only one unit test - just add it to the filter.
	// testing::GTEST_FLAG(filter) = "SomeTest*";

	return RUN_ALL_TESTS();
}
