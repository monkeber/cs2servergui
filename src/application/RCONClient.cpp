#include "RCONClient.h"

#include <QtLogging>

#include <AppData.h>

RCONClient::RCONClient(QObject* parent)
	: QObject{ parent }
	, m_client{ nullptr }
{
	connect(
		AppData::Instance().settings.get(), &Settings::rconPassChanged, this, &RCONClient::Reset);
	connect(
		AppData::Instance().settings.get(), &Settings::rconPortChanged, this, &RCONClient::Reset);
	connect(
		AppData::Instance().settings.get(), &Settings::serverIPChanged, this, &RCONClient::Reset);
}

void RCONClient::Exec(const QString& cmd)
{
	if (!m_client)
	{
		Reset();
	}

	m_client->send_data(cmd.toStdString(), 999, rconpp::data_type::SERVERDATA_EXECCOMMAND);
}

void RCONClient::Reset()
{
	m_client.reset(new rconpp::rcon_client{
		AppData::Instance().settings->getServerIP().toStdString(),
		AppData::Instance().settings->getRconPort(),
		AppData::Instance().settings->getRconPass().toStdString(),
	});

	m_client->on_log = [](const std::string_view& log) {
		qWarning(std::string{ log }.c_str());
	};

	m_client->start(true);
}
