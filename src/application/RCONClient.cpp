#include "RCONClient.h"

#include <QtLogging>

#include <AppData.h>

RCONClient::RCONClient(QObject* parent)
	: QObject{ parent }
	, m_client{ nullptr }
{
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
	m_client.reset(
		new rconpp::rcon_client{ AppData::Instance().settings->getServerIP().toStdString(),
			27015,
			AppData::Instance().settings->getRconPass().toStdString() });

	m_client->on_log = [](const std::string_view& log) {
		qWarning(std::string{ log }.c_str());
	};

	m_client->start(true);
}
