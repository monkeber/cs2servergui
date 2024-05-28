#include "Settings.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtLogging>

namespace
{
namespace details
{

static QString ConfigFilePath{ "config.json" };

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
	const QJsonDocument doc{ QJsonDocument::fromJson(jsonFile.readAll()) };
	jsonFile.close();
	if (!doc.isObject())
	{
		qWarning("Config data is ill-formed, skipping");
		return;
	}

	const QJsonObject obj{ doc.object() };
	m_executablePath = obj.value("executablePath").toString("");
	m_rconPass = obj.value("rconPass").toString("");
	m_serverIP = obj.value("serverIP").toString("");
	m_startParameters = obj.value("startParameters").toString("");
}

Settings::~Settings()
{
	try
	{
		QJsonObject obj;
		obj.insert("executablePath", m_executablePath);
		obj.insert("rconPass", m_rconPass);
		obj.insert("serverIP", m_serverIP);
		obj.insert("startParameters", m_startParameters);

		QJsonDocument doc;
		doc.setObject(obj);

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

QString Settings::getRconPass() const
{
	return m_rconPass;
}

QString Settings::getServerIP() const
{
	return m_serverIP;
}

QString Settings::getStartParameters() const
{
	return m_startParameters;
}

void Settings::setExecutablePath(QString fileUrl)
{
	const QUrl url{ fileUrl };
	m_executablePath = QDir::toNativeSeparators(url.toLocalFile());

	emit executablePathChanged(m_executablePath);
}
