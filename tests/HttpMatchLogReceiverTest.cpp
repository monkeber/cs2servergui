#include "gtest/gtest.h"

#include <match/HttpLogReceiver.h>

#include <httplib.h>

#include <future>

TEST(HttpMatchLogReceiverTest, PassesPostBodyToHandler)
{
	constexpr std::uint16_t port{ 27599 };
	std::promise<std::string> received;
	HttpMatchLogReceiver receiver{ port, [&received](std::string chunk) {
									  received.set_value(std::move(chunk));
								  } };

	httplib::Client client{ "127.0.0.1", port };
	const auto response{ client.Post("/log", "line 1\nline 2\n", "text/plain") };

	ASSERT_TRUE(response);
	ASSERT_EQ(response->status, 200);
	ASSERT_EQ(received.get_future().get(), "line 1\nline 2\n");
	ASSERT_EQ(receiver.ServerStartParams(),
		R"(+log on +logaddress_add_http "http://127.0.0.1:27599/log")");
}

TEST(HttpMatchLogReceiverTest, ThrowsWhenPortIsTaken)
{
	constexpr std::uint16_t port{ 27598 };
	HttpMatchLogReceiver first{ port, [](std::string) {} };

	ASSERT_THROW((HttpMatchLogReceiver{ port, [](std::string) {} }), std::runtime_error);
}
