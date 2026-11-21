#pragma once

#include <filesystem>
#include <fstream>
#include <string_view>

//! Dev-only tool to study what the game server actually sends. Appends every received log chunk
//! unchanged to one file per app run, with a marker line before each chunk.
class RawLogWriter
{
public:
	//! Creates the directory if needed. Throws if the file cannot be opened.
	explicit RawLogWriter(const std::filesystem::path& directory);

	const std::filesystem::path& FilePath() const;

	void WriteChunk(std::string_view chunk);
	void WriteNote(std::string_view note);

private:
	void StartMarkerLine();

private:
	std::filesystem::path m_filePath;
	std::ofstream m_file;
	int m_chunkCount{ 0 };
	bool m_isAtLineStart{ true };
};
