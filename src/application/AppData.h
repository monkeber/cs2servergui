#pragma once

#include "ProcessHandler.h"
#include "RCONClient.h"
#include "Settings.h"

class AppData : public QObject
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	Q_PROPERTY(ProcessHandler* serverProcess READ serverProcess CONSTANT)
	Q_PROPERTY(Settings* settings READ settings CONSTANT)

public:
	static AppData& Instance();
	static AppData* create(QQmlEngine*, QJSEngine* engine);

	RCONClient* rconclient();
	ProcessHandler* serverProcess();
	Settings* settings();

private:
	AppData();

private:
	ProcessHandler m_serverProcess;
	RCONClient m_rconclient;
	Settings m_settings;
};
