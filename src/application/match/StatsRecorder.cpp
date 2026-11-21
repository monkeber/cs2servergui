#include "StatsRecorder.h"

#include "Cs2LogParser.h"
#include "HttpLogReceiver.h"
#include "SqliteStatsStore.h"

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
		m_store = std::make_unique<SqliteMatchStatsStore>();
		m_parser = std::make_unique<Cs2MatchLogParser>();
		m_service = std::make_unique<MatchStatsService>(*m_parser, *m_store);
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
	return m_source ? m_source->ServerStartParams() : std::string{};
}

void MatchStatsRecorder::HandleLog(const std::string& chunk)
{
	try
	{
		m_service->HandleLog(chunk);
	}
	catch (const std::exception& e)
	{
		qWarning("Failed to record match stats: %s", e.what());
	}
}
