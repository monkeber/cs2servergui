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
