#include "LogReceiver.h"

#include <httplib.h>

#include <format>
#include <stdexcept>

std::string MatchLogServerParams(std::uint16_t port)
{
	// The URL is quoted, otherwise the game console splits it on ':'.
	return std::format(R"(+log on +logaddress_add_http "http://127.0.0.1:{}/log")", port);
}

MatchLogReceiver::MatchLogReceiver(std::uint16_t port, LogHandler handler)
	: m_server{ std::make_unique<httplib::Server>() }
{
	// A single worker keeps the chunks in the order the server sent them.
	m_server->new_task_queue = [] {
		return new httplib::ThreadPool{ 1 };
	};
	// The httplib default (SO_REUSEPORT, or SO_REUSEADDR on Windows) lets a second app instance
	// bind the same port and take part of the log stream. Fail loudly instead.
	m_server->set_socket_options([](socket_t sock) {
#ifdef _WIN32
		httplib::set_socket_opt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, 1);
#else
		httplib::set_socket_opt(sock, SOL_SOCKET, SO_REUSEADDR, 1);
#endif
	});
	m_server->Post("/log",
		[handler = std::move(handler)](const httplib::Request& request, httplib::Response&) {
			handler(request.body);
		});

	if (!m_server->bind_to_port("127.0.0.1", port))
	{
		throw std::runtime_error{ std::format("Cannot listen on port {} for match logs", port) };
	}

	m_thread = std::thread{ [this] {
		m_server->listen_after_bind();
	} };
}

MatchLogReceiver::~MatchLogReceiver()
{
	m_server->stop();
	m_thread.join();
}
