#include "AppData.h"

AppData::AppData()
	: m_log{}
	, m_mapHistory{}
	, m_serverProcess{}
	, m_rconclient{}
	, m_settings{}
	, m_matchStatsDb{}
	, m_matchStats{ m_matchStatsDb }
{
	StartMatchLogReceiver();
}

AppData& AppData::Instance()
{
	static AppData app;

	return app;
}

AppData* AppData::create(QQmlEngine*, QJSEngine* engine)
{
	Q_ASSERT(engine->thread() == Instance().thread());
	QJSEngine::setObjectOwnership(&Instance(), QJSEngine::CppOwnership);

	return &Instance();
}

Log* AppData::log()
{
	return &m_log;
}

MapHistory* AppData::mapHistory()
{
	return &m_mapHistory;
}

RCONClient* AppData::rconclient()
{
	return &m_rconclient;
}

ProcessHandler* AppData::serverProcess()
{
	return &m_serverProcess;
}

Settings* AppData::settings()
{
	return &m_settings;
}

void AppData::StartMatchLogReceiver()
{
	try
	{
		m_matchLogReceiver =
			std::make_unique<MatchLogReceiver>(kMatchLogPort, [this](std::string chunk) {
				// The receiver calls us from its own thread, the stats are handled on the main one.
				QMetaObject::invokeMethod(
					this,
					[this, chunk = std::move(chunk)] {
						HandleMatchLog(chunk);
					},
					Qt::QueuedConnection);
			});
	}
	catch (const std::exception& e)
	{
		qWarning("Match stats are disabled: %s", e.what());
	}
}

void AppData::HandleMatchLog(const std::string& chunk)
{
	try
	{
		m_matchStats.HandleLog(chunk);
	}
	catch (const std::exception& e)
	{
		qWarning("Failed to store match stats: %s", e.what());
	}
}
