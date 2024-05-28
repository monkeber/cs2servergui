#include "ProcessHandler.h"
#include "AppData.h"

#include <QFileInfo>
#include <QtLogging>

#include <windows.h>

BOOL CALLBACK SendWMCloseMsg(HWND hwnd, LPARAM lParam)
{
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hwnd, &dwProcessId);
	if (dwProcessId == lParam)
	{
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	}

	return TRUE;
}

ProcessHandler::ProcessHandler(QObject* parent)
	: QObject{ parent }
	, m_isRunning{ false }
{
}

void ProcessHandler::execCommand(const QString& cmd)
{
	AppData::Instance().rconclient->Exec(cmd);
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

	const QFileInfo file{ AppData::Instance().settings->getExecutablePath() };

	if (!CreateProcess(NULL,							   // No module name (use command line)
			command.data(),								   // Command line
			NULL,										   // Process handle not inheritable
			NULL,										   // Thread handle not inheritable
			FALSE,										   // Set handle inheritance to FALSE
			0,											   // No creation flags
			NULL,										   // Use parent's environment block
			file.absolutePath().toStdWString().c_str(),	   // Use the parent directory of a chosen
														   // executable as a starting directory
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
	EnumWindows(&SendWMCloseMsg, std::any_cast<PROCESS_INFORMATION>(m_processInfo).dwProcessId);
	if (WaitForSingleObject(std::any_cast<PROCESS_INFORMATION>(m_processInfo).hProcess, 500)
		== WAIT_TIMEOUT)
	{
		TerminateProcess(std::any_cast<PROCESS_INFORMATION>(m_processInfo).hProcess, 0);
	}
	CloseHandle(std::any_cast<PROCESS_INFORMATION>(m_processInfo).hProcess);
	CloseHandle(std::any_cast<PROCESS_INFORMATION>(m_processInfo).hThread);

	m_isRunning = false;
	emit runningStateChanged(m_isRunning);
}

void ProcessHandler::readData()
{
	emit outputChanged(m_output);
}

void ProcessHandler::readErrors()
{
	emit outputChanged(m_output);
}
