#pragma once

#include <QObject>
#include <QtGlobal>
#include <QQmlEngine>

void InitLogging();

void SetOriginalHandler(QtMessageHandler handler);

class Log : public QObject
{
	Q_OBJECT
	QML_ELEMENT

signals:
	void newLogMessageCaptured(const QString& newmsg);

public:
	Log(QObject* parent = nullptr);
};

/*
 * The reason for existence of both LogHandler and Log classes is because previously I had a
 * standalone MessageOutput function which accessed the AppData singletone and it seemed like it did
 * so before main QML thread, which led to Q_ASSERT fail `Q_ASSERT(engine->thread() ==
 * Instance().thread());` in `AppData::create`. So I created a singleton object which is not used
 * anywhere else and serves just as a midlle man. And Log class exists just for connecting signals
 * in QML. Maybe there is a better way of doing this whole setup.
 * LogHandler should not be used anywhere else.
 */
class LogHandler : public QObject
{
	Q_OBJECT

signals:
	void newLogReceived(const QString& newmsg);

public:
	static void MessageOutput(
		QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
	static LogHandler& Instance();
	LogHandler() = default;

	friend Log;
};
