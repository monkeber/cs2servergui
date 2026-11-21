#include "RawLogWriter.h"

#include <chrono>
#include <format>
#include <stdexcept>

RawLogWriter::RawLogWriter(const std::filesystem::path& directory)
{
	std::filesystem::create_directories(directory);

	const auto now{ std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()) };
	m_filePath = directory / std::format("match_log_{:%Y%m%d_%H%M%S}_utc.log", now);
	m_file.open(m_filePath, std::ios::binary | std::ios::app);
	if (!m_file)
	{
		throw std::runtime_error{ std::format("Cannot open {}", m_filePath.string()) };
	}
}

const std::filesystem::path& RawLogWriter::FilePath() const
{
	return m_filePath;
}

//! Before: chunk `L1\nL2` (no trailing newline)
//! After:  `### chunk 1 | 5 bytes | ends with newline: no\nL1\nL2`
void RawLogWriter::WriteChunk(std::string_view chunk)
{
	const bool endsWithNewline{ chunk.ends_with('\n') };

	StartMarkerLine();
	m_file << std::format("### chunk {} | {} bytes | ends with newline: {}\n",
		++m_chunkCount,
		chunk.size(),
		endsWithNewline ? "yes" : "no");
	m_file << chunk;
	m_file.flush();

	m_isAtLineStart = chunk.empty() || endsWithNewline;
}

void RawLogWriter::WriteNote(std::string_view note)
{
	StartMarkerLine();
	m_file << "### " << note << '\n';
	m_file.flush();

	m_isAtLineStart = true;
}

void RawLogWriter::StartMarkerLine()
{
	// Marker lines must stay on their own line, also after a chunk without a trailing newline.
	if (!m_isAtLineStart)
	{
		m_file << '\n';
	}
}
