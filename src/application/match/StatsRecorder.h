#pragma once

#include "LogParser.h"
#include "LogSource.h"
#include "RawLogWriter.h"
#include "StatsService.h"
#include "StatsStore.h"

#include <QObject>

#include <memory>
#include <string>

//! Composition root of match stats recording. Picks the implementations, moves received logs to
//! the main thread and keeps failures away from the rest of the app.
class MatchStatsRecorder : public QObject
{
public:
	//! With `saveRawLogs` every received chunk is also written to a file, for dev tests only.
	explicit MatchStatsRecorder(bool saveRawLogs, QObject* parent = nullptr);

	//! Game server start parameters that enable recording, empty when recording is disabled.
	std::string ServerStartParams() const;
	//! Starts over, for when the game server starts or stops.
	void ResetSession();

private:
	void HandleLog(const std::string& chunk);

private:
	std::unique_ptr<MatchStatsStore> m_store;
	std::unique_ptr<MatchLogParser> m_parser;
	std::unique_ptr<MatchStatsService> m_service;
	std::unique_ptr<RawLogWriter> m_rawLog;
	//! Declared last so it stops before the components it feeds are destroyed.
	std::unique_ptr<MatchLogSource> m_source;
};
