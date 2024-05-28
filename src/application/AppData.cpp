#include "AppData.h"

AppData::AppData()
	: serverProcess{ std::make_unique<ProcessHandler>() }
	, rconclient{ std::make_unique<RCONClient>() }
	, settings{ std::make_unique<Settings>() }
{
}

AppData& AppData::Instance()
{
	static AppData app;

	return app;
}
