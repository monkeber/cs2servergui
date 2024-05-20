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
	Q_PROPERTY(QString startParameters MEMBER m_startParameters NOTIFY startParametersChanged)
public:
	explicit Settings(QObject* parent = nullptr);
	~Settings();

signals:
	void executablePathChanged(const QString& newPath);
	void startParametersChanged(const QString& newPath);

public:
	QString getExecutablePath() const;
	QString getStartParameters() const;

public slots:
	void setExecutablePath(QString fileUrl);

private:
	QString m_executablePath;
	QString m_startParameters;
};
