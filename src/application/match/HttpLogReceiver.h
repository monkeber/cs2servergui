#pragma once

#include "LogSource.h"

#include <cstdint>
#include <memory>
#include <thread>

namespace httplib
{
class Server;
}

//! Local HTTP endpoint for the CS2 log stream (`logaddress_add_http`). The handler runs on the
//! receiver thread, one request at a time.
class HttpMatchLogReceiver final : public MatchLogSource
{
public:
	//! Throws if the port cannot be bound.
	HttpMatchLogReceiver(std::uint16_t port, LogHandler handler);
	~HttpMatchLogReceiver() override;

	HttpMatchLogReceiver(const HttpMatchLogReceiver&) = delete;
	HttpMatchLogReceiver& operator=(const HttpMatchLogReceiver&) = delete;

	std::string ServerStartParams() const override;

private:
	std::uint16_t m_port;
	std::unique_ptr<httplib::Server> m_server;
	std::thread m_thread;
};
