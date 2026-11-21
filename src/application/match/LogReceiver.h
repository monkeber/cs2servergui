#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <thread>

namespace httplib
{
class Server;
}

inline constexpr std::uint16_t kMatchLogPort{ 27500 };

//! Server start parameters that make CS2 stream its log to the receiver.
std::string MatchLogServerParams(std::uint16_t port = kMatchLogPort);

//! Local HTTP endpoint for the CS2 log stream (`logaddress_add_http`). The handler runs on the
//! receiver thread, one request at a time, so chunks keep their order.
class MatchLogReceiver
{
public:
	using LogHandler = std::function<void(std::string chunk)>;

	//! Throws if the port cannot be bound.
	MatchLogReceiver(std::uint16_t port, LogHandler handler);
	~MatchLogReceiver();

	MatchLogReceiver(const MatchLogReceiver&) = delete;
	MatchLogReceiver& operator=(const MatchLogReceiver&) = delete;

private:
	std::unique_ptr<httplib::Server> m_server;
	std::thread m_thread;
};
