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
		qWarning("Config data is ill-formed, skipping.");
	}

	const QJsonObject obj{ doc.object() };
	m_executablePath = obj.value("executablePath").toString("");
	m_startParameters = obj.value("startParameters").toString("");
}

Settings::~Settings()
{
	try
	{
		QJsonObject obj;
		obj.insert("executablePath", m_executablePath);
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
		qWarning("Caught exception while saving the config:", e.what());
	}
	catch (...)
	{
		qWarning("Caught unknown exception while saving the config");
	}
}

QString Settings::getExecutablePath() const
{
	return m_executablePath;
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
