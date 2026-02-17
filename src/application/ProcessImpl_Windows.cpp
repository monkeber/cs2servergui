#include <ProcessImpl.h>

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

bool ProcessImpl::start(const std::filesystem::path& executablePath, const std::string& startParams)
{
	m_startupInfo = STARTUPINFO();
	m_processInfo = PROCESS_INFORMATION();

	ZeroMemory(std::any_cast<STARTUPINFO>(&m_startupInfo),
		sizeof(std::any_cast<STARTUPINFO>(m_startupInfo)));
	std::any_cast<STARTUPINFO&>(m_startupInfo).cb =
		sizeof(std::any_cast<STARTUPINFO>(m_startupInfo));
	ZeroMemory(std::any_cast<PROCESS_INFORMATION>(&m_processInfo),
		sizeof(std::any_cast<PROCESS_INFORMATION>(m_processInfo)));

	std::string command{ "\"" + executablePath.string() + "\" " + startParams };
	if (!CreateProcess(NULL,						 // No module name (use command line)
			command.data(),							 // Command line
			NULL,									 // Process handle not inheritable
			NULL,									 // Thread handle not inheritable
			FALSE,									 // Set handle inheritance to FALSE
			0,										 // No creation flags
			NULL,									 // Use parent's environment block
			executablePath.parent_path().c_str(),	 // Use the parent directory of a chosen
													 // executable as a starting directory
			std::any_cast<STARTUPINFO>(&m_startupInfo),
			std::any_cast<PROCESS_INFORMATION>(&m_processInfo)))
	{
		qWarning("CreateProcess failed (%d)", static_cast<int>(GetLastError()));
		return false;
	}

	return true;
}

void ProcessImpl::stop()
{
	EnumWindows(&SendWMCloseMsg, std::any_cast<PROCESS_INFORMATION>(m_processInfo).dwProcessId);
	if (WaitForSingleObject(std::any_cast<PROCESS_INFORMATION>(m_processInfo).hProcess, 500)
		== WAIT_TIMEOUT)
	{
		TerminateProcess(std::any_cast<PROCESS_INFORMATION>(m_processInfo).hProcess, 0);
	}
	CloseHandle(std::any_cast<PROCESS_INFORMATION>(m_processInfo).hProcess);
	CloseHandle(std::any_cast<PROCESS_INFORMATION>(m_processInfo).hThread);
}
