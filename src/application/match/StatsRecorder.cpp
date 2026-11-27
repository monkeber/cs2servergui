#include "StatsRecorder.h"

#include "HttpLogReceiver.h"

#include <QCoreApplication>
#include <QtLogging>

namespace
{

constexpr std::uint16_t kLogPort{ 27500 };

}	 // namespace

MatchStatsRecorder::MatchStatsRecorder(QObject* parent)
	: QObject{ parent }
{
	try
	{
		m_rawLog = std::make_unique<RawLogWriter>(
			std::filesystem::path{ QCoreApplication::applicationDirPath().toStdWString() } / "matches");
		qInfo("Match logs are written to %s", m_rawLog->FilePath().string().c_str());
	}
	catch (const std::exception& e)
	{
		qWarning("Match logs are disabled: %s", e.what());
		return;
	}

	try
	{
		m_source = std::make_unique<HttpMatchLogReceiver>(kLogPort, [this](std::string chunk) {
			// The source calls us from its own thread, the logs are written on the main one.
			QMetaObject::invokeMethod(
				this,
				[this, chunk = std::move(chunk)] {
					HandleLog(chunk);
				},
				Qt::QueuedConnection);
		});
	}
	catch (const std::exception& e)
	{
		qWarning("Match log recording is disabled: %s", e.what());
	}
}

std::string MatchStatsRecorder::ServerStartParams() const
{
	if (!m_source)
	{
		return {};
	}

	// Damage lines are only logged with this detail level.
	return m_source->ServerStartParams() + " +mp_logdetail 3";
}

void MatchStatsRecorder::ResetSession()
{
	// Nothing more is expected of the previous session, so a held back block is complete as it is.
	WritePieces(m_splitter.Flush());
	m_splitter.Reset();
	m_roundStats.Reset();

	if (m_rawLog)
	{
		m_rawLog->WriteNote("game server started or stopped");
	}
}

void MatchStatsRecorder::HandleLog(const std::string& chunk)
{
	WritePieces(m_splitter.Split(chunk));
}

void MatchStatsRecorder::WritePieces(const std::vector<MatchLogSplitter::Piece>& pieces)
{
	if (!m_rawLog)
	{
		return;
	}

	for (const auto& piece : pieces)
	{
		if (piece.m_startsNewMatch)
		{
			StartNewMatchFile();
		}

		m_rawLog->WriteChunk(piece.m_text);
		for (const auto& line : m_roundStats.Feed(piece.m_text))
		{
			m_rawLog->WriteLine(line);
			if (line.starts_with("### check: missing"))
			{
				qWarning("Match log %s: %s", m_rawLog->FilePath().string().c_str(), line.c_str());
			}
		}
	}
}

void MatchStatsRecorder::StartNewMatchFile()
{
	m_roundStats.StartNewMatch();

	try
	{
		m_rawLog->StartNewFile();
		qInfo("Match logs are written to %s", m_rawLog->FilePath().string().c_str());
	}
	catch (const std::exception& e)
	{
		// The log of this match is lost, the rest of the app is not affected.
		qWarning("Cannot start a log file for the new match: %s", e.what());
	}
}
