#include <QtTest/QTest>

#include <iostream>

#include <rconpp/rcon.h>

class TestProcessPipe : public QObject
{
	Q_OBJECT
private slots:
	void run();
};

void TestProcessPipe::run()
{
	rconpp::rcon_client client("178.151.133.64", 27015, "passpass");

	client.on_log = [](const std::string_view& log) {
		std::cout << log << std::endl;
	};

	client.start(true);

	client.send_data("map de_inferno",
		3,
		rconpp::data_type::SERVERDATA_EXECCOMMAND,
		[](const rconpp::response& response) {
			std::cout << "response: " << response.data << std::endl;
		});
	std::this_thread::sleep_for(std::chrono::seconds{ 5 });
}

QTEST_MAIN(TestProcessPipe)
#include "main.moc"
