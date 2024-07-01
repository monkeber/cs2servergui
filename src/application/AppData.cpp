#include "AppData.h"

AppData::AppData()
	: m_serverProcess{}
	, m_rconclient{}
	, m_settings{}
{
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
