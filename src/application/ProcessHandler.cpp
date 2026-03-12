#include "ProcessHandler.h"
#include "ProcessImpl.h"
#include "AppData.h"
#include "MapHistory.h"

#include <QFileInfo>
#include <QRegularExpression>
#include <QtLogging>
#include <QUrlQuery>

#include <cpr/cpr.h>

ProcessHandler::ProcessHandler(QObject* parent)
	: QObject{ parent }
	, m_isRunning{ false }
	, m_generalHistory{ this }
	, m_scriptHistory{ this }
	, m_process{ std::make_unique<ProcessImpl>() }
{
}

ProcessHandler::~ProcessHandler() = default;

CommandHistory* ProcessHandler::getGeneralHistory()
{
	return &m_generalHistory;
}

CommandHistory* ProcessHandler::getScriptHistory()
{
	return &m_scriptHistory;
}

void ProcessHandler::execCommand(const QString& cmd, const bool recordInGeneralHistory)
{
	AppData::Instance().rconclient()->Exec(cmd);
	if (recordInGeneralHistory)
	{
		m_generalHistory.add(cmd);
	}

	if (cmd.trimmed().startsWith("host_workshop_map"))
	{
		const auto argList{ cmd.trimmed().split(" ", Qt::SkipEmptyParts) };
		if (argList.size() > 1)
		{
			const auto boundCall{ std::bind(
				&MapHistory::Add, AppData::Instance().mapHistory(), argList.at(1).toStdString()) };
			std::thread worker{ boundCall };
			worker.detach();
		}
	}
}

void ProcessHandler::execScriptName(const QString& scriptName)
{
	execCommand(QString{ "exec %1" }.arg(scriptName), false);
	m_scriptHistory.add(scriptName);
}

void ProcessHandler::resetRconConnection()
{
	// A common error is the game server is not responding after it gets restarted, so in case
	// of an error we simply reset the rcon handler. Alas there is no good solution to do it
	// automatically with this version of the library.
	AppData::Instance().rconclient()->Reset();
}

void ProcessHandler::hostWorkshopMap(const QString& map)
{
	constexpr qsizetype sizeLimit{ 1000 };
	if (map.size() > sizeLimit)
	{
		qWarning("Received map link or id exceeds size limit, size: %lld, limit: %lld",
			map.size(),
			sizeLimit);
	}

	QString mapId;
	// If the provided string contains only numbers we assume its just a map id.
	static const QRegularExpression re{ "^\\d*$" };
	if (re.match(map).hasMatch())
	{
		mapId = map;
	}
	// Otherwise we try to parse a link from steam workshop.
	else
	{
		const QUrl url{ map };
		if (!url.isValid())
		{
			qWarning("Provided url is not valid: %s", qUtf8Printable(map));
			return;
		}

		const QUrlQuery query{ url };
		mapId = query.queryItemValue("id");
		if (mapId.isEmpty())
		{
			qWarning("Provided url does not contain id: %s", qUtf8Printable(map));
			return;
		}
	}

	execCommand(QString{ "host_workshop_map %1" }.arg(mapId), false);
}

void ProcessHandler::start()
{
	if (!m_process->start(AppData::Instance().settings()->getExecutablePath().toStdString(),
			AppData::Instance().settings()->getStartParameters().toStdString()))
	{
		return;
	}

	m_isRunning = true;

	emit runningStateChanged(m_isRunning);
}

void ProcessHandler::stop()
{
	m_process->stop();

	m_isRunning = false;
	emit runningStateChanged(m_isRunning);
}
