#pragma once

#include "LogSource.h"
#include "MatchLogSplitter.h"
#include "RawLogWriter.h"
#include "RoundStatsExtractor.h"

#include <QObject>

#include <memory>
#include <string>
#include <vector>

//! Composition root of match log recording. Writes the log of the game server to `raw_logs/`, one
//! file per match, as it is received. The only lines added are one-line `ROUND_STATS` versions of
//! the round statistics and a check that none is missing. Moves received logs to the main thread
//! and keeps failures away from the rest of the app.
class MatchStatsRecorder : public QObject
{
public:
	explicit MatchStatsRecorder(QObject* parent = nullptr);

	//! Game server start parameters that enable recording, empty when recording is disabled.
	std::string ServerStartParams() const;
	//! Starts over, for when the game server starts or stops.
	void ResetSession();

private:
	void HandleLog(const std::string& chunk);
	void WritePieces(const std::vector<MatchLogSplitter::Piece>& pieces);
	void StartNewMatchFile();

private:
	MatchLogSplitter m_splitter;
	RoundStatsExtractor m_roundStats;
	std::unique_ptr<RawLogWriter> m_rawLog;
	//! Declared last so it stops before the components it feeds are destroyed.
	std::unique_ptr<MatchLogSource> m_source;
};
