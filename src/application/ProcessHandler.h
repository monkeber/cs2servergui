#pragma once

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
	void setFilePath(QString filePath);
	void setStartPatameters(QString startParameters);
	void start();
	void stop();

private slots:
	void readData();
	void readErrors();

private:
	QString m_filePath;
	QString m_startParameters;
	QString m_output;
	QProcess m_process;
	bool m_isRunning;
};
