#include <ProcessImpl.h>

#include <QtLogging>

#include <signal.h>
#include <unistd.h>

bool ProcessImpl::start(const std::filesystem::path& executablePath, const std::string& startParams)
{
	pid_t processID = 0;
	std::string terminal;
	if (access("xterm", X_OK) == 0)
	{
		terminal = "xterm";
	}
	else if (access("/usr/bin/xterm", X_OK) == 0)
	{
		terminal = "/usr/bin/xterm";
	}
	else
	{
		qWarning("Could not find xterm, please either install it or start the server manually");
		return false;
	}

	processID = fork();
	if (processID == 0)
	{
		std::string filepath{ executablePath.string() };
		std::string params{ startParams };
		std::string termOptions{ "-e" };

		char* argV[]{ terminal.data(), termOptions.data(), filepath.data(), params.data(), nullptr };
		execvp(terminal.c_str(), argV);

		std::exit(1);
	}
	else if (processID < 0)
	{
		qWarning("Failed while forking the process, error: '%s'", strerror(errno));
		return false;
	}

	m_processInfo = processID;

	return true;
}

void ProcessImpl::stop()
{
	kill(std::any_cast<pid_t>(m_processInfo), SIGKILL);
}
