#include "ProcessHandler.h"
#include "AppData.h"

#include <QFileInfo>
#include <QtLogging>

#include <windows.h>

ProcessHandler::ProcessHandler(QObject* parent)
	: QObject{ parent }
	, m_isRunning{ false }
{
}

void ProcessHandler::execCommand(const QString& cmd)
{
	// const qint64 result = m_process.write(cmd.toUtf8());
	// if (result < 1)
	// {
	// 	qWarning("Something went wrong while writing to the process, bytes written:", result);
	// }
}

void ProcessHandler::start()
{
	m_startupInfo = STARTUPINFO();
	m_processInfo = PROCESS_INFORMATION();

	ZeroMemory(std::any_cast<STARTUPINFO>(&m_startupInfo),
		sizeof(std::any_cast<STARTUPINFO>(m_startupInfo)));
	std::any_cast<STARTUPINFO&>(m_startupInfo).cb =
		sizeof(std::any_cast<STARTUPINFO>(m_startupInfo));
	ZeroMemory(std::any_cast<PROCESS_INFORMATION>(&m_processInfo),
		sizeof(std::any_cast<PROCESS_INFORMATION>(m_processInfo)));

	const std::wstring startParams{
		AppData::Instance().settings->getStartParameters().toStdWString()
	};

	std::wstring command{ L"\"" + AppData::Instance().settings->getExecutablePath().toStdWString()
		+ L"\" " + startParams };

	if (!CreateProcess(NULL,							   // No module name (use command line)
			command.data(),								   // Command line
			NULL,										   // Process handle not inheritable
			NULL,										   // Thread handle not inheritable
			FALSE,										   // Set handle inheritance to FALSE
			0,											   // No creation flags
			NULL,										   // Use parent's environment block
			NULL,										   // Use parent's starting directory
			std::any_cast<STARTUPINFO>(&m_startupInfo),	   // Pointer to STARTUPINFO structure
			std::any_cast<PROCESS_INFORMATION>(
				&m_processInfo))	// Pointer to PROCESS_INFORMATION structure
	)
	{
		qWarning("CreateProcess failed (%d)", static_cast<int>(GetLastError()));
		return;
	}

	m_isRunning = true;

	emit runningStateChanged(m_isRunning);
}

void ProcessHandler::stop()
{
	CloseHandle(std::any_cast<PROCESS_INFORMATION>(m_processInfo).hProcess);
	CloseHandle(std::any_cast<PROCESS_INFORMATION>(m_processInfo).hThread);
	// m_process.terminate();
	m_isRunning = false;
	emit runningStateChanged(m_isRunning);
}

void ProcessHandler::readData()
{
	// m_output = m_process.readAllStandardOutput();
	emit outputChanged(m_output);
}

void ProcessHandler::readErrors()
{
	// m_output = m_process.readAllStandardError();
	emit outputChanged(m_output);
}
