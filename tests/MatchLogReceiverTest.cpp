#include "gtest/gtest.h"

#include <match/LogReceiver.h>

#include <httplib.h>

#include <future>

TEST(MatchLogReceiverTest, PassesPostBodyToHandler)
{
	constexpr std::uint16_t port{ 27599 };
	std::promise<std::string> received;
	MatchLogReceiver receiver{ port, [&received](std::string chunk) {
								  received.set_value(std::move(chunk));
							  } };

	httplib::Client client{ "127.0.0.1", port };
	const auto response{ client.Post("/log", "line 1\nline 2\n", "text/plain") };

	ASSERT_TRUE(response);
	ASSERT_EQ(response->status, 200);
	ASSERT_EQ(received.get_future().get(), "line 1\nline 2\n");
}

TEST(MatchLogReceiverTest, ThrowsWhenPortIsTaken)
{
	constexpr std::uint16_t port{ 27598 };
	MatchLogReceiver first{ port, [](std::string) {} };

	ASSERT_THROW((MatchLogReceiver{ port, [](std::string) {} }), std::runtime_error);
}
