#pragma once

#include <functional>
#include <string>

//! Delivers the raw game server log. Implementations call the handler they were created with for
//! every received chunk, in the order the server sent them.
class MatchLogSource
{
public:
	using LogHandler = std::function<void(std::string chunk)>;

	virtual ~MatchLogSource() = default;

	//! Game server start parameters that make it send its log to this source.
	virtual std::string ServerStartParams() const = 0;
};
