#include "ProcessHandler.h"

ProcessHandler::ProcessHandler(QObject* parent)
	: QObject{ parent }
	, m_process{ this }
	, m_isRunning{ false }
{
	connect(&m_process, &QProcess::readyReadStandardOutput, this, &ProcessHandler::readData);
	connect(&m_process, &QProcess::readyReadStandardError, this, &ProcessHandler::readErrors);
}

void ProcessHandler::start()
{
	// m_process.startCommand("ping -n 10 google.com");
	m_process.startCommand(R"__(
"W:\Games\steamapps\common\Counter-Strike Global Offensive\game\bin\win64\cs2.exe"
-dedicated -usercon +game_type 0 +game_mode 1 +map de_dust2 -dev)__");
	m_isRunning = true;
	emit runningStateChanged(m_isRunning);
}

void ProcessHandler::stop()
{
	m_process.terminate();
	m_isRunning = false;
	emit runningStateChanged(m_isRunning);
}

void ProcessHandler::readData()
{
	m_output = m_process.readAllStandardOutput();
	// m_output.append(m_process.readAllStandardOutput());
	emit outputChanged(m_output);
}

void ProcessHandler::readErrors()
{
	m_output = m_process.readAllStandardError();
	// m_output.append(m_process.readAllStandardError());
	emit outputChanged(m_output);
}
