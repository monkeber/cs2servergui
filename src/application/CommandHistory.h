#pragma once

#include <QObject>

class CommandHistory : public QObject
{
	Q_OBJECT

public:
	CommandHistory(QObject* parent = nullptr);

public slots:
	//! Returns a newer or an older record in the history, changes currentIndex and returns	empty
	//! string if we reach either of the ends of history.
	QString getNewer();
	QString getOlder();

public:
	//! Adds a new command into the history and resets the current index. Doesn record new command
	//! if its the same as the previous one.
	void add(const QString& cmd);

private:
	qsizetype m_currentIndex;
	//! Grows at the end. At the begin() is the oldest record, and before the end() is the newest
	//! record.
	QList<QString> m_history;
};
