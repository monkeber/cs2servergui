#include "ProcessHandler.h"
#include "AppData.h"

#include <QFileInfo>
#include <QtLogging>

ProcessHandler::ProcessHandler(QObject* parent)
	: QObject{ parent }
	, m_process{ this }
	, m_isRunning{ false }
{
	connect(&m_process, &QProcess::readyReadStandardOutput, this, &ProcessHandler::readData);
	connect(&m_process, &QProcess::readyReadStandardError, this, &ProcessHandler::readErrors);
}

void ProcessHandler::execCommand(const QString& cmd)
{
	const qint64 result = m_process.write(cmd.toUtf8());
	if (result < 1)
	{
		qWarning("Something went wrong while writing to the process, bytes written:", result);
	}
}

void ProcessHandler::start()
{
	m_process.setProgram(AppData::Instance().settings->getExecutablePath());
	m_process.setNativeArguments(AppData::Instance().settings->getStartParameters());
	const QFileInfo file{ AppData::Instance().settings->getExecutablePath() };
	m_process.setWorkingDirectory(file.absolutePath());
	m_process.setInputChannelMode(QProcess::InputChannelMode::ForwardedInputChannel);
	m_process.start();
	m_process.closeWriteChannel();

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
