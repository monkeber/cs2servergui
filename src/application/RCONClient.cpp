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
	if (!m_client || !m_client->connected.load())
	{
		Reset();
	}

	m_client->send_data(cmd.toStdString(), 999, rconpp::data_type::SERVERDATA_EXECCOMMAND);
}

void RCONClient::Reset()
{
	// Use shared pointers in case the client gets reset again before detached thread finishes.
	m_client = std::make_shared<rconpp::rcon_client>(
		AppData::Instance().settings->getServerIP().toStdString(),
		AppData::Instance().settings->getRconPort(),
		AppData::Instance().settings->getRconPass().toStdString());

	m_client->on_log = [](const std::string_view& log) {
		qWarning(std::string{ log }.c_str());
	};

	// Avoid hanging the main app thread.
	std::thread startingThread{ [client = this->m_client]() {
		client->start(true);
	} };
	startingThread.detach();
}
