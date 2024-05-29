#pragma once

#include <QObject>
#include <QQmlEngine>

class Settings : public QObject
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	Q_PROPERTY(QString executablePath READ getExecutablePath WRITE setExecutablePath NOTIFY
			executablePathChanged)
	Q_PROPERTY(QString rconPass MEMBER m_rconPass NOTIFY rconPassChanged)
	Q_PROPERTY(quint16 rconPort MEMBER m_rconPort NOTIFY rconPortChanged)
	Q_PROPERTY(QString serverIP MEMBER m_serverIP NOTIFY serverIPChanged)
	Q_PROPERTY(QString startParameters MEMBER m_startParameters NOTIFY startParametersChanged)
public:
	explicit Settings(QObject* parent = nullptr);
	~Settings();

signals:
	void executablePathChanged(const QString& newPath);
	void rconPassChanged(const QString& newRconPass);
	void rconPortChanged(const quint16& newRconPort);
	void startParametersChanged(const QString& newPath);
	void serverIPChanged(const QString& newServerIP);

public:
	QString getExecutablePath() const;
	QString getRconPass() const;
	quint16 getRconPort() const;
	QString getServerIP() const;
	QString getStartParameters() const;

public slots:
	void setExecutablePath(QString fileUrl);

private:
	QString m_executablePath;
	QString m_rconPass;
	quint16 m_rconPort;
	QString m_serverIP;
	QString m_startParameters;
};
