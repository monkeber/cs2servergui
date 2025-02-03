#pragma once

#include <QObject>
#include <QQmlEngine>

class Settings : public QObject
{
	Q_OBJECT
	QML_ELEMENT

	Q_PROPERTY(QString executablePath READ getExecutablePath WRITE setExecutablePath NOTIFY
			executablePathChanged)
	Q_PROPERTY(QStringList quickCommands MEMBER m_quickCommands NOTIFY quickCommandsChanged)
	Q_PROPERTY(QString rconPass MEMBER m_rconPass NOTIFY rconPassChanged)
	Q_PROPERTY(quint16 rconPort MEMBER m_rconPort NOTIFY rconPortChanged)
	Q_PROPERTY(qreal scaleFactor MEMBER m_scaleFactor NOTIFY scaleFactorChanged)
	Q_PROPERTY(QString serverIP MEMBER m_serverIP NOTIFY serverIPChanged)
	Q_PROPERTY(QString startParameters MEMBER m_startParameters NOTIFY startParametersChanged)
	Q_PROPERTY(QString theme MEMBER m_theme NOTIFY themeChanged)
public:
	explicit Settings(QObject* parent = nullptr);
	~Settings();

signals:
	void executablePathChanged(const QString& newPath);
	void quickCommandsChanged(const QStringList& newQuickCommands);
	void rconPassChanged(const QString& newRconPass);
	void rconPortChanged(const quint16& newRconPort);
	void scaleFactorChanged(const qreal& newScaleFactor);
	void serverIPChanged(const QString& newServerIP);
	void startParametersChanged(const QString& newPath);
	void themeChanged(const QString& newThemeName);

public:
	QString getExecutablePath() const;
	QStringList getQuickCommands() const;
	QString getRconPass() const;
	quint16 getRconPort() const;
	qreal getScaleFactor() const;
	QString getServerIP() const;
	QString getStartParameters() const;
	QString getTheme() const;

public slots:
	void setExecutablePath(QString fileUrl);

private:
	void fromJson(const nl::json js);
	nl::json toJson() const;

private:
	QString m_executablePath;
	QStringList m_quickCommands;
	QString m_rconPass;
	quint16 m_rconPort = 0;
	qreal m_scaleFactor = 1.0;
	QString m_serverIP;
	QString m_startParameters;
	QString m_theme;
};
