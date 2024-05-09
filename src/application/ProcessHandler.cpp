#include "ProcessHandler.h"

#include <QFileInfo>

#include <iostream>

ProcessHandler::ProcessHandler(QObject* parent)
	: QObject{ parent }
	, m_process{ this }
	, m_isRunning{ false }
{
	connect(&m_process, &QProcess::readyReadStandardOutput, this, &ProcessHandler::readData);
	connect(&m_process, &QProcess::readyReadStandardError, this, &ProcessHandler::readErrors);
}

void ProcessHandler::setFilePath(QString filePath)
{
	if (filePath.startsWith("file:///"))
	{
		filePath = filePath.remove(0, 8);
	}
	m_filePath = filePath;
	QFileInfo info{ m_filePath };
	std::cout << info.absolutePath().toStdString() << std::endl;
	std::cout << filePath.toStdString() << std::endl;
}

void ProcessHandler::setStartPatameters(QString startParameters)
{
	m_startParameters = startParameters;
}

void ProcessHandler::start()
{
	// m_process.startCommand("ping -n 10 google.com");
	// 	m_process.startCommand(R"__(
	// "W:\Games\steamapps\common\Counter-Strike Global Offensive\game\bin\win64\cs2.exe"
	// -dedicated -usercon +game_type 0 +game_mode 1 +map de_dust2 -dev)__");

	QString cmd{ "\"" + m_filePath + "\" " + m_startParameters };
	m_process.startCommand(cmd);
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
