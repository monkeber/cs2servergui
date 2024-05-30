#pragma once

#include <any>

#include <QObject>
#include <QProcess>
#include <QQmlEngine>

class ProcessHandler : public QObject
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON
	Q_PROPERTY(QString output MEMBER m_output NOTIFY outputChanged)
	Q_PROPERTY(bool isRunning MEMBER m_isRunning NOTIFY runningStateChanged)

signals:
	void outputChanged(const QString& newText);
	void runningStateChanged(const bool isRunning);

public:
	ProcessHandler(QObject* parent = nullptr);

public slots:
	void execCommand(const QString& cmd);
	void execScriptName(const QString& scriptName);
	void hostWorkshopMap(const QString& map);
	void start();
	void stop();

private slots:
	void readData();
	void readErrors();

private:
	QString m_filePath;
	QString m_startParameters;
	QString m_output;
	bool m_isRunning;
	std::any m_processInfo;
	std::any m_startupInfo;
};
