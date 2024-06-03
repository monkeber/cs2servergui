#include "CommandHistory.h"

CommandHistory::CommandHistory(QObject* parent)
	: QObject{ parent }
	, m_currentIndex{ 0 }
{
}

QString CommandHistory::getNewer()
{
	if (m_history.empty() || m_currentIndex == 0)
	{
		return "";
	}
	// When the current index is 1 it means we are on the newest command, so the "newer" could be
	// only an empty string.
	if (m_currentIndex == 1)
	{
		--m_currentIndex;
		return "";
	}

	return m_history.at(m_history.size() - (--m_currentIndex));
}

QString CommandHistory::getOlder()
{
	if (m_history.empty())
	{
		return "";
	}
	// Don't allow to go older than the oldest command.
	if (m_currentIndex == m_history.size())
	{
		return m_history.first();
	}

	return m_history.at(m_history.size() - (++m_currentIndex));
}

void CommandHistory::add(const QString& cmd)
{
	m_currentIndex = 0;
	if (!m_history.empty() && cmd == m_history.last())
	{
		return;
	}

	m_history.push_back(cmd);
}
