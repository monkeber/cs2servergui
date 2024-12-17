#pragma once

#include <QObject>
#include <QtGlobal>
#include <QQmlEngine>

void InitLogging();

void MessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg);

void SetOriginalHandler(QtMessageHandler handler);

class Log : public QObject
{
	Q_OBJECT
	QML_ELEMENT

signals:
	void newLogMessageCaptured(const QString& newmsg);

public:
	Log(QObject* parent = nullptr);

public:
	void AddNewMessage(const QString& newmsg);
};
