#pragma once

#include <memory>

#include "ProcessHandler.h"
#include "Settings.h"

class AppData
{
public:
	static AppData& Instance();

public:
	std::unique_ptr<ProcessHandler> serverProcess;
	std::unique_ptr<Settings> settings;

private:
	AppData();
};
