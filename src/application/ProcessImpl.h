#pragma once

#include <any>

class ProcessImpl
{
public:
	//! Starts the process specified by the executable path with the provided start params. Returns
	//! true if the process was started successfully.
	bool start(const std::filesystem::path& executablePath, const std::string& startParams);
	//! Stops the previously started process.
	void stop();

private:
	std::any m_processInfo;
	std::any m_startupInfo;
};
