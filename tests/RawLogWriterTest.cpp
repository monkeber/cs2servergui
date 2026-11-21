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

	std::ifstream file{ filePath, std::ios::binary };
	std::stringstream content;
	content << file.rdbuf();

	ASSERT_EQ(content.str(),
		"### chunk 1 | 13 bytes | ends with newline: no\n"
		"line 1\nline 2\n"
		"### game server started or stopped\n"
		"### chunk 2 | 7 bytes | ends with newline: yes\n"
		"line 3\n");

	std::filesystem::remove_all(directory);
}
