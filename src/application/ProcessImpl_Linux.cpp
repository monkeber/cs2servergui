#include <ProcessImpl.h>

#include <QtLogging>

#include <signal.h>
#include <spawn.h>

bool ProcessImpl::start(const std::filesystem::path& executablePath, const std::string& startParams)
{
	pid_t processID = 0;
	std::string path{ executablePath.string() };
	std::string filename{ executablePath.filename().string() };
	std::string params{ startParams };
	char* argV[]{ filename.data(), params.data(), (char*)0 };
	const int status{ posix_spawn(&processID, path.c_str(), NULL, NULL, argV, environ) };

	m_processInfo = processID;

	return status == 0;
}

void ProcessImpl::stop()
{
	kill(std::any_cast<pid_t>(m_processInfo), SIGTERM);
}
