#pragma once

#include <any>

#include <QObject>
#include <QQmlEngine>

#include <CommandHistory.h>

class ProcessHandler : public QObject
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON
	Q_PROPERTY(bool isRunning MEMBER m_isRunning NOTIFY runningStateChanged)

signals:
	void runningStateChanged(const bool isRunning);

public:
	ProcessHandler(QObject* parent = nullptr);

public slots:
	CommandHistory* getGeneralHistory();
	CommandHistory* getScriptHistory();
	//! Sends the provided command for execution on the game server. Can record the provided command
	//! in general command history if the second parameter is true.
	void execCommand(const QString& cmd, const bool recordInGeneralHistory = true);
	//! Sends a command in the format of `exec <scriptName>`, this is just a shortcut for
	//! convenience.
	void execScriptName(const QString& scriptName);
	//! Sends a host_workshop_map command, the map parameter should be either map id from workshop
	//! or a URL of the map, in case of URL - it will be parsed and map id extracted from url
	//! parameters.
	void hostWorkshopMap(const QString& map);
	//! Starts the executable which is specified in app settings.
	void start();
	//! Stop the executable.
	void stop();

private:
	bool m_isRunning;
	std::any m_processInfo;
	std::any m_startupInfo;
	CommandHistory m_generalHistory;
	CommandHistory m_scriptHistory;
};
