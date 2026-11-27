#pragma once

#include <QtLogging>

#include <exception>
#include <utility>

//! Runs one step of match recording so that nothing it throws reaches the rest of the app: a
//! failed recording must never get in the way of playing. Logs a warning for the first failure
//! of a streak only (`hasReported` is kept by the caller), a failing disk would otherwise add a
//! line for every received chunk. Returns whether the step succeeded.
template <typename Step>
bool RunGuarded(const char* what, bool& hasReported, Step&& step)
{
	try
	{
		step();
		hasReported = false;
		return true;
	}
	catch (const std::exception& e)
	{
		if (!std::exchange(hasReported, true))
		{
			qWarning("Match recording, %s failed, the match log may be incomplete: %s",
				what,
				e.what());
		}
	}
	catch (...)
	{
		if (!std::exchange(hasReported, true))
		{
			qWarning("Match recording, %s failed with an unknown error, the match log may be "
					 "incomplete",
				what);
		}
	}
	return false;
}
