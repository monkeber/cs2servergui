#include "StatsRecorder.h"

#include "Cs2LogParser.h"
#include "HttpLogReceiver.h"
#include "SqliteStatsStore.h"

#include <QtLogging>

namespace
{

constexpr std::uint16_t kLogPort{ 27500 };

}	 // namespace

MatchStatsRecorder::MatchStatsRecorder(bool saveRawLogs, QObject* parent)
	: QObject{ parent }
{
	if (saveRawLogs)
	{
		try
		{
			m_rawLog = std::make_unique<RawLogWriter>("raw_logs");
			qInfo("Raw match logs are written to %s", m_rawLog->FilePath().string().c_str());
		}
		catch (const std::exception& e)
		{
			qWarning("Raw match logs are disabled: %s", e.what());
		}
	}

	try
	{
		m_store = std::make_unique<SqliteMatchStatsStore>();
		m_parser = std::make_unique<Cs2MatchLogParser>();
		m_service = std::make_unique<MatchStatsService>(*m_parser, *m_store);
	}
	catch (const std::exception& e)
	{
		qWarning("Match stats storage is disabled: %s", e.what());
	}

	// Raw logs are still worth receiving when the storage failed.
	if (!m_service && !m_rawLog)
	{
		return;
	}

	try
	{
		m_source = std::make_unique<HttpMatchLogReceiver>(kLogPort, [this](std::string chunk) {
			// The source calls us from its own thread, the stats are handled on the main one.
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
		qWarning("Match stats recording is disabled: %s", e.what());
	}
}

std::string MatchStatsRecorder::ServerStartParams() const
{
	if (!m_source)
	{
		return {};
	}

	// Damage lines are only logged with this detail level, raw logs should show everything.
	return m_rawLog ? m_source->ServerStartParams() + " +mp_logdetail 3"
					: m_source->ServerStartParams();
}

void MatchStatsRecorder::ResetSession()
{
	if (m_rawLog)
	{
		m_rawLog->WriteNote("game server started or stopped");
	}

	if (m_service)
	{
		m_service->Reset();
	}
}

void MatchStatsRecorder::HandleLog(const std::string& chunk)
{
	if (m_rawLog)
	{
		m_rawLog->WriteChunk(chunk);
	}

	if (!m_service)
	{
		return;
	}

	try
	{
		m_service->HandleLog(chunk);
	}
	catch (const std::exception& e)
	{
		qWarning("Failed to record match stats: %s", e.what());
	}
}
