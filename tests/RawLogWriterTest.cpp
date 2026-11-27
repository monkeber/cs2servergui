#include "gtest/gtest.h"

#include <match/RawLogWriter.h>

#include <fstream>
#include <sstream>

TEST(RawLogWriterTest, KeepsChunksUnchangedWithMarkers)
{
	const auto directory{ std::filesystem::temp_directory_path() / "cs2servergui_raw_log_test" };
	std::filesystem::remove_all(directory);

	std::filesystem::path filePath;
	{
		RawLogWriter writer{ directory };
		filePath = writer.FilePath();
		writer.WriteChunk("line 1\nline 2");
		writer.WriteNote("game server started or stopped");
		writer.WriteChunk("line 3\n");
	}

	std::stringstream content;
	{
		// Must be closed before the cleanup below, Windows cannot delete files that are still open.
		std::ifstream file{ filePath, std::ios::binary };
		content << file.rdbuf();
	}

	// Clean up before asserting, so a failed assertion doesn't leave the directory behind.
	std::filesystem::remove_all(directory);

	ASSERT_EQ(content.str(),
		"### chunk 1 | 13 bytes | ends with newline: no\n"
		"line 1\nline 2\n"
		"### game server started or stopped\n"
		"### chunk 2 | 7 bytes | ends with newline: yes\n"
		"line 3\n");
}

TEST(RawLogWriterTest, StartNewFileSwitchesFilesOnlyWhenCurrentOneHasContent)
{
	const auto directory{ std::filesystem::temp_directory_path() / "cs2servergui_raw_log_test_new" };
	std::filesystem::remove_all(directory);

	std::filesystem::path first;
	std::filesystem::path second;
	std::filesystem::path third;
	{
		RawLogWriter writer{ directory };
		first = writer.FilePath();
		writer.StartNewFile();
		const bool keptEmptyFile{ writer.FilePath() == first };

		writer.WriteChunk("match 1\n");
		writer.StartNewFile();
		second = writer.FilePath();
		writer.WriteChunk("match 2\n");
		writer.StartNewFile();
		third = writer.FilePath();

		EXPECT_TRUE(keptEmptyFile);
	}

	const auto distinct{ first != second && second != third && first != third };
	const auto count{ std::distance(std::filesystem::directory_iterator{ directory },
		std::filesystem::directory_iterator{}) };
	std::filesystem::remove_all(directory);

	EXPECT_TRUE(distinct);
	EXPECT_EQ(count, 3);
}

TEST(RawLogWriterTest, WritesOwnLinesOnLinesOfTheirOwn)
{
	const auto directory{ std::filesystem::temp_directory_path() / "cs2servergui_raw_log_test_line" };
	std::filesystem::remove_all(directory);

	std::filesystem::path filePath;
	{
		RawLogWriter writer{ directory };
		filePath = writer.FilePath();
		// The chunk has no trailing newline, our line must not be glued to it.
		writer.WriteChunk("line 1");
		writer.WriteLine("ROUND_STATS {}");
		writer.WriteChunk("line 2\n");
	}

	std::stringstream content;
	{
		std::ifstream file{ filePath, std::ios::binary };
		content << file.rdbuf();
	}
	std::filesystem::remove_all(directory);

	ASSERT_EQ(content.str(),
		"### chunk 1 | 6 bytes | ends with newline: no\n"
		"line 1\n"
		"ROUND_STATS {}\n"
		"### chunk 2 | 7 bytes | ends with newline: yes\n"
		"line 2\n");
}
