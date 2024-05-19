#include "ProcessHandler.h"
#include "AppData.h"

#include <QFileInfo>

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
	m_process.setProgram(AppData::Instance().settings->getExecutablePath());
	m_process.setNativeArguments(AppData::Instance().settings->GetStartParameters());
	const QFileInfo file{ AppData::Instance().settings->getExecutablePath() };
	m_process.setWorkingDirectory(file.absolutePath());
	m_process.start();

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
	emit outputChanged(m_output);
}

void ProcessHandler::readErrors()
{
	m_output = m_process.readAllStandardError();
	emit outputChanged(m_output);
}
