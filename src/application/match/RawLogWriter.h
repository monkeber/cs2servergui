#pragma once

#include <filesystem>
#include <fstream>
#include <string_view>

//! Appends every received log chunk unchanged to one file per match, with a marker line before
//! each chunk, so what the game server actually sent can be told from what was added later.
class RawLogWriter
{
public:
	//! Creates the directory if needed. Throws if the file cannot be opened.
	explicit RawLogWriter(const std::filesystem::path& directory);

	const std::filesystem::path& FilePath() const;

	//! Continues in a new file, for the next match. Does nothing while the current file is empty.
	void StartNewFile();

	void WriteChunk(std::string_view chunk);
	//! A line of our own, on a line of its own (without a trailing newline).
	void WriteLine(std::string_view line);
	void WriteNote(std::string_view note);

private:
	void OpenFile();
	void StartMarkerLine();
	void CheckStream() const;

private:
	std::filesystem::path m_directory;
	std::filesystem::path m_filePath;
	std::ofstream m_file;
	int m_chunkCount{ 0 };
	bool m_isAtLineStart{ true };
	bool m_hasChunks{ false };
};
