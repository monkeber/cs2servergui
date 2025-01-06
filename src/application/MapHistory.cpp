#include "MapHistory.h"

#include <QtLogging>

#include <cpr/cpr.h>
#include <rapidcsv.h>

MapHistory::MapHistory()
{
}

void MapHistory::Add(const std::string& mapId)
{
	const nl::json resp = GetMapInfo(mapId);

	const nl::json::json_pointer infoPath{ "/reposne/publishedfiledetails/0" };
	if (!resp.contains(infoPath))
	{
		qWarning(
			"Was not able to access the path '%s' in the response from Steam API with the body: %s",
			infoPath.to_string().c_str(),
			resp.dump().c_str());

		return;
	}

	const nl::json info = resp.at(infoPath);

	const std::string previewUrlField{ "preview_url" };
	if (!resp.contains(previewUrlField))
	{
		qWarning("Was not able to find the '%s' in the response from Steam API with the body: %s",
			previewUrlField.c_str(),
			resp.dump().c_str());

		return;
	}
	const std::string previewUrl{ info.at(previewUrlField) };

	const std::string titleField{ "title" };
	if (!resp.contains(previewUrlField))
	{
		qWarning("Was not able to find the '%s' in the response from Steam API with the body: %s",
			titleField.c_str(),
			resp.dump().c_str());

		return;
	}
	const std::string mapName{ info.at(titleField) };

	const auto filePath{ DownloadPreview(mapId, previewUrl) };

	SaveMapEntry(mapId, mapName, filePath.relative_path().string());
}

std::filesystem::path MapHistory::DownloadPreview(const std::string& mapId, const std::string& url)
{
	using namespace std::literals::string_literals;

	std::filesystem::path filepath{ "previews" };
	if (!std::filesystem::exists(filepath) || !std::filesystem::is_directory(filepath))
	{
		std::filesystem::create_directory(filepath);
	}

	filepath.append("preview_"s + mapId + ".jpeg");
	std::ofstream of{ filepath, std::ios::out | std::ios::binary };
	cpr::Download(of, cpr::Url{ url });

	return filepath;
}

nl::json MapHistory::GetMapInfo(const std::string& mapId)
{
	static const std::string url{
		"https://api.steampowered.com/ISteamRemoteStorage/"
		"GetPublishedFileDetails/v1/?format=json"
	};
	const cpr::Response r = cpr::Post(cpr::Url{ url },
		cpr::Multipart{
			{ "itemcount", "1" },
			{ "publishedfileids[0]", mapId },
		});

	if (r.status_code != 200)
	{
		qWarning("Request to Steam API returned not OK response, code: %d, body: &s",
			r.status_code,
			r.text.c_str());
	}

	return nl::json::parse(r.text);
}

void MapHistory::SaveMapEntry(
	const std::string& mapId, const std::string& mapName, const std::string& previewPath)
{
	static const std::filesystem::path historyFilePath{ "map_history.csv" };
	if (!std::filesystem::exists(historyFilePath)
		|| !std::filesystem::is_regular_file(historyFilePath))
	{
		std::ofstream of{ historyFilePath, std::ios::out };
		of << "Map Workshop ID,Map Name,Downloaded At,PreviewPath\n";
		of.close();
	}

	std::ofstream of{ historyFilePath, std::ios::app };
	const std::chrono::zoned_time currentTime{ std::chrono::current_zone(),
		std::chrono::system_clock::now() };

	of << mapId << "," << mapName << "," << currentTime << "," << previewPath << "\n";

	of.close();
}
