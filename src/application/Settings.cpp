#include "Settings.h"

#include <QDir>
#include <QtLogging>

#include <fstream>

namespace
{
namespace details
{

const char* ConfigFilePath{ "config.json" };

//! Returns data and a boolean, boolean has true value if everything is OK and false if the file
//! does not exist or any error has occured while retrieving the data.
std::pair<nl::json, bool> OpenAndReadFile()
{
	const std::filesystem::path filepath{ ConfigFilePath };
	if (!std::filesystem::exists(filepath) || !std::filesystem::is_regular_file(filepath))
	{
		return { nl::json::object(), false };
	}

	std::ifstream configFile{ filepath };
	std::stringstream dataStream;
	dataStream << configFile.rdbuf();

	const std::string data{ dataStream.str() };
	if (!nl::json::accept(dataStream))
	{
		qWarning("Cannot parse config data, skipping");
		return { nl::json::object(), false };
	}

	return { nl::json::parse(data), true };
}

//! Writes the provided data to a file.
void WriteFile(const nl::json& data)
{
	const std::filesystem::path filepath{ ConfigFilePath };
	std::ofstream configFile{ filepath };
	configFile << data.dump(1);
}

}	 // namespace details
}	 // namespace

Settings::Settings(QObject* parent)
	: QObject{ parent }
{
	const auto [data, isOK] = details::OpenAndReadFile();

	if (isOK)
	{
		fromJson(data);
	}
}

Settings::~Settings()
{
	try
	{
		details::WriteFile(toJson());
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

void Settings::fromJson(const nl::json js)
{
	try
	{
		if (!js.is_object())
		{
			qWarning("Config data is ill-formed, skipping");
			return;
		}

		if (js.contains("server"))
		{
			// Settings related to server configuration.
			const nl::json server = js.at("server");
			m_executablePath = server.value("executablePath", "").c_str();
			m_rconPass = server.value("rconPass", "").c_str();
			m_rconPort = server.value("rconPort", 0);
			m_serverIP = server.value("serverIP", "").c_str();
			m_startParameters = server.value("startParameters", "").c_str();
			for (const auto& str : server.value<std::vector<std::string>>("quickCommands", {}))
			{
				m_quickCommands.push_back(str.c_str());
			}
		}
		else
		{
			qWarning("Config data is ill-formed, missing 'server' field");
		}

		if (js.contains("application"))
		{
			// Settings related to config of the app itself - scale, theme, etc.
			const nl::json app = js.at("application");
			m_scaleFactor = app.value<double>("scalingFactor", 1.0);
			m_theme = app.value("theme", "").c_str();
		}
		else
		{
			qWarning("Config data is ill-formed, missing 'application' field");
		}
	}
	catch (const std::exception& e)
	{
		qWarning("Config data is ill-formed, exception: %s", e.what());
	}
	catch (...)
	{
		qWarning("Config data is ill-formed, unknown exception");
	}
}

nl::json Settings::toJson() const
{
	nl::json server;
	server["executablePath"] = m_executablePath.toStdString();
	server["rconPass"] = m_rconPass.toStdString();
	server["rconPort"] = m_rconPort;
	server["serverIP"] = m_serverIP.toStdString();
	server["startParameters"] = m_startParameters.toStdString();

	for (const auto& cmd : m_quickCommands)
	{
		server["quickCommands"].push_back(cmd.toStdString());
	}

	nl::json app;
	app["scalingFactor"] = m_scaleFactor;
	app["theme"] = m_theme.toStdString();

	nl::json obj;
	obj["server"] = server;
	obj["application"] = app;

	return obj;
}
