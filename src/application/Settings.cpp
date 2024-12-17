#include "Settings.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtLogging>

namespace
{
namespace details
{

const QString ConfigFilePath{ "config.json" };

}	 // namespace details
}	 // namespace

Settings::Settings(QObject* parent)
	: QObject{ parent }
{
	QFile jsonFile{ details::ConfigFilePath };
	if (!jsonFile.exists())
	{
		return;
	}

	jsonFile.open(QFile::ReadOnly);
	fromJson(std::move(QJsonDocument::fromJson(jsonFile.readAll())));
	jsonFile.close();
}

Settings::~Settings()
{
	try
	{
		const QJsonDocument doc = toJson();

		QFile jsonFile{ details::ConfigFilePath };
		jsonFile.open(QFile::WriteOnly);
		jsonFile.write(doc.toJson());
		jsonFile.close();
	}
	catch (const std::exception& e)
	{
		qWarning("Exception while saving the config:", e.what());
	}
	catch (...)
	{
		qWarning("Unknown exception while saving the config");
	}
}

QString Settings::getExecutablePath() const
{
	return m_executablePath;
}

QStringList Settings::getQuickCommands() const
{
	return m_quickCommands;
}

QString Settings::getRconPass() const
{
	return m_rconPass;
}

quint16 Settings::getRconPort() const
{
	return m_rconPort;
}

qreal Settings::getScaleFactor() const
{
	return m_scaleFactor;
}

QString Settings::getServerIP() const
{
	return m_serverIP;
}

QString Settings::getStartParameters() const
{
	return m_startParameters;
}

QString Settings::getTheme() const
{
	return m_theme;
}

void Settings::setExecutablePath(QString fileUrl)
{
	const QUrl url{ fileUrl };
	m_executablePath = QDir::toNativeSeparators(url.toLocalFile());

	emit executablePathChanged(m_executablePath);
}

void Settings::fromJson(const QJsonDocument json)
{
	if (!json.isObject())
	{
		qWarning("Config data is ill-formed, skipping");
		return;
	}

	const QJsonObject obj{ json.object() };

	// Settings related to server configuration.
	const QJsonObject server{ obj.value("server").toObject() };
	m_executablePath = server.value("executablePath").toString("");
	m_rconPass = server.value("rconPass").toString("");
	m_rconPort = server.value("rconPort").toInt();
	m_serverIP = server.value("serverIP").toString("");
	m_startParameters = server.value("startParameters").toString("");
	m_quickCommands = server.value("quickCommands").toVariant().toStringList();

	// Settings related to config of the app itself - scale, theme, etc.
	const QJsonObject app{ obj.value("application").toObject() };
	m_scaleFactor = app.value("scalingFactor").toDouble(1.0);
	m_theme = app.value("theme").toString("");
}

QJsonDocument Settings::toJson() const
{
	QJsonObject server;
	server.insert("executablePath", m_executablePath);
	server.insert("rconPass", m_rconPass);
	server.insert("rconPort", m_rconPort);
	server.insert("serverIP", m_serverIP);
	server.insert("startParameters", m_startParameters);
	if (!m_quickCommands.empty())
	{
		server.insert("quickCommands", QJsonArray::fromStringList(m_quickCommands));
	}

	QJsonObject app;
	app.insert("scalingFactor", m_scaleFactor);
	app.insert("theme", m_theme);

	QJsonObject obj;
	obj.insert("server", server);
	obj.insert("application", app);

	QJsonDocument doc;
	doc.setObject(obj);

	return doc;
}
