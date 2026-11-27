#include "RawLogWriter.h"

#include <chrono>
#include <format>
#include <stdexcept>

RawLogWriter::RawLogWriter(const std::filesystem::path& directory)
	: m_directory{ directory }
{
	std::filesystem::create_directories(m_directory);
	OpenFile();
}

void RawLogWriter::OpenFile()
{
	const auto now{ std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()) };
	const auto baseName{ std::format("{:%Y%m%d_%H%M%S}_utc", now) };

	// Two files can start within the same second.
	m_filePath = m_directory / (baseName + ".log");
	for (int suffix{ 2 }; std::filesystem::exists(m_filePath); ++suffix)
	{
		m_filePath = m_directory / std::format("{}_{}.log", baseName, suffix);
	}

	m_file.open(m_filePath, std::ios::binary | std::ios::app);
	if (!m_file)
	{
		throw std::runtime_error{ std::format("Cannot open {}", m_filePath.string()) };
	}
}

void RawLogWriter::StartNewFile()
{
	if (!m_hasChunks)
	{
		return;
	}

	m_file.close();
	m_file.clear();
	m_chunkCount = 0;
	m_isAtLineStart = true;
	m_hasChunks = false;
	OpenFile();
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
	m_hasChunks = true;

	m_isAtLineStart = chunk.empty() || endsWithNewline;
	CheckStream();
}

void RawLogWriter::WriteLine(std::string_view line)
{
	StartMarkerLine();
	m_file << line << '\n';
	m_file.flush();

	m_isAtLineStart = true;
	CheckStream();
}

void RawLogWriter::WriteNote(std::string_view note)
{
	StartMarkerLine();
	m_file << "### " << note << '\n';
	m_file.flush();

	m_isAtLineStart = true;
	CheckStream();
}

//! A stream that went bad (disk full, file removed) only sets a flag, make it visible.
void RawLogWriter::CheckStream() const
{
	if (!m_file)
	{
		throw std::runtime_error{ std::format("Cannot write to {}", m_filePath.string()) };
	}
}

void RawLogWriter::StartMarkerLine()
{
	// Marker lines must stay on their own line, also after a chunk without a trailing newline.
	if (!m_isAtLineStart)
	{
		m_file << '\n';
	}
}
