#include <QtTest/QTest>

#include <fstream>
#include <string>

#include <rconpp/rcon.h>
#include <cpr/cpr.h>

class TestProcessPipe : public QObject
{
	Q_OBJECT
private slots:
	void run();
};

void TestProcessPipe::run()
{
	// rconpp::rcon_client client("178.151.133.64", 27015, "passpass");

	// client.on_log = [](const std::string_view& log) {
	// 	std::cout << log << std::endl;
	// };

	// client.start(true);

	// client.send_data("map de_inferno",
	// 	3,
	// 	rconpp::data_type::SERVERDATA_EXECCOMMAND,
	// 	[](const rconpp::response& response) {
	// 		std::cout << "response: " << response.data << std::endl;
	// 	});
	// std::this_thread::sleep_for(std::chrono::seconds{ 5 });

	using namespace std::literals::string_literals;

	const std::string mapId{ "3386673040" };

	static const std::string url{
		"https://api.steampowered.com/ISteamRemoteStorage/"
		"GetPublishedFileDetails/v1/?format=json"
	};
	cpr::Response r = cpr::Post(cpr::Url{ url },
		cpr::Multipart{
			{ "itemcount", "1" },
			{ "publishedfileids[0]", mapId },
		});

	std::cout << r.text << std::endl;
	std::cout << r.status_code << std::endl;
	std::cout << r.error.message << std::endl;
	std::cout << static_cast<int>(r.error.code) << std::endl;

	std::cout << "Fedora:" << std::endl;

	r = cpr::Get(cpr::Url{ "https://fedoraproject.org/static/hotspot.txt" });

	std::cout << r.text << std::endl;
	std::cout << r.status_code << std::endl;
	std::cout << r.error.message << std::endl;
	std::cout << static_cast<int>(r.error.code) << std::endl;

	std::cout << "Downloading file:" << std::endl;

	std::filesystem::path path{ "previews" };
	std::filesystem::create_directory(path);
	path.append("preview_"s + mapId + ".jpeg");
	std::ofstream of{ path, std::ios::out | std::ios::binary };
	r = cpr::Download(of,
		cpr::Url{
			"https://steamuserimages-a.akamaihd.net/ugc/62585168915727855/"
			"FD99BD79546C0BB6788B2596DD7B334E0263BD11/",
		});

	// cpr::Response r = cpr::Get(cpr::Url{ "https://fedoraproject.org/static/hotspot.txt" });

	// std::cout << r.text << std::endl;
	std::cout << r.status_code << std::endl;
	std::cout << r.error.message << std::endl;
	std::cout << static_cast<int>(r.error.code) << std::endl;
}

QTEST_MAIN(TestProcessPipe)
#include "main.moc"
