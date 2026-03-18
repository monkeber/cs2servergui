#include "MapHistory.h"

#include <QtLogging>

#include <cpr/cpr.h>

#include <exception>

MapHistory::MapHistory(QObject* parent)
	: QObject{ parent }
{
	m_modelRef = QVariant::fromValue(&m_model);

	QObject::connect(this, &MapHistory::EntriesAdded, &m_model, &MapHistoryModel::AddEntries);
	QObject::connect(this, &MapHistory::ResetHistory, &m_model, &MapHistoryModel::ClearModel);

	QObject::connect(
		&m_model, &MapHistoryModel::RemoveMapEntry, this, &MapHistory::RemoveMapEntries);
	QObject::connect(
		&m_model, &MapHistoryModel::UpdateBookmarkedSignal, this, &MapHistory::UpdateMapBookmarked);
	QObject::connect(
		&m_model, &MapHistoryModel::UpdateFilters, this, &MapHistory::ReloadFileWithFilters);
	QObject::connect(
		&m_model, &MapHistoryModel::UpdateRatingSignal, this, &MapHistory::UpdateMapRating);

	ReloadFile();
}

void MapHistory::RemoveMapEntries(const std::string& workshopId, const std::string& playedAt)
{
	const auto entry{ m_db.SelectOne(workshopId, playedAt) };
	m_db.Delete(workshopId, playedAt);
	if (!m_db.Exists(workshopId))
	{
		std::filesystem::remove(std::filesystem::path{ entry.m_previewPath });
	}
}

void MapHistory::UpdateMapBookmarked(const std::string& workshopId, const bool isBookmarked)
{
	MapHistoryPatch patch;
	patch.m_isBookmarked = isBookmarked;
	m_db.Update(workshopId, patch);
	ReloadFile();
}

void MapHistory::UpdateMapRating(const std::string& workshopId, const std::uint8_t rating)
{
	MapHistoryPatch patch;
	patch.m_rating = rating;
	m_db.Update(workshopId, patch);
	ReloadFile();
}

void MapHistory::Add(const std::string& mapId)
{
	if (m_db.Exists(mapId))
	{
		m_db.AddJournalEntry(mapId);
	}
	else
	{
		try
		{
			const auto [mapName, previewUrl] = GetMapNameAndPreviewUrl(mapId);
			const auto filePath{ DownloadPreview(mapId, previewUrl) };
			m_db.AddNewMap(mapId, mapName, filePath.relative_path().generic_string());
		}
		catch (const std::exception& e)
		{
			qWarning("Encountered an error when adding a new map with ID %s: %s",
				mapId.c_str(),
				e.what());
			return;
		}
		catch (...)
		{
			qWarning(
				"Encountered an unknown error when adding a new map with ID %s", mapId.c_str());
			return;
		}
	}

	ReloadFile();
}

void MapHistory::FixPreviews()
{
	const auto lambda = [this]() {
		try
		{
			MapHistoryFilters filters;
			filters.m_removeDuplicated = true;
			std::vector<MapHistoryEntry> maps{ m_db.Select(filters) };

			const auto doesExist = [](const MapHistoryEntry& entry) {
				return std::filesystem::exists(entry.m_previewPath);
			};
			std::erase_if(maps, doesExist);

			if (maps.empty())
			{
				return;
			}

			for (const auto& entry : maps)
			{
				try
				{
					const auto [_, previewUrl] = GetMapNameAndPreviewUrl(entry.m_workshopID);
					DownloadPreview(entry.m_workshopID, previewUrl);
				}
				catch (const std::exception& e)
				{
					qWarning("Encountered an error when fixing preview for workshop ID %s: %s",
						entry.m_workshopID.c_str(),
						e.what());
				}
				catch (...)
				{
					qWarning("Encountered an unknown error when fixing preview for workshop ID %s",
						entry.m_workshopID.c_str());
				}
			}
			ReloadFile();
		}
		catch (const std::exception& e)
		{
			qWarning("Error encountered while trying to fix the missing previews: %s", e.what());
		}
		catch (...)
		{
			qWarning("Unknown error encountered while trying to fix the missing previews");
		}
	};

	std::thread worker{ lambda };
	worker.detach();
}

std::filesystem::path MapHistory::DownloadPreview(const std::string& mapId, const std::string& url)
{
	std::filesystem::path filepath{ "previews" };
	if (!std::filesystem::exists(filepath) || !std::filesystem::is_directory(filepath))
	{
		std::filesystem::create_directory(filepath);
	}

	filepath.append("preview_" + mapId + ".jpeg");
	std::ofstream of{ filepath, std::ios::out | std::ios::binary };
	cpr::Download(of, cpr::Url{ url });

	return filepath;
}

std::pair<std::string, std::string> MapHistory::GetMapNameAndPreviewUrl(const std::string& mapId)
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
		qWarning("Request to Steam API returned not OK response, code: %ld, body: %s",
			r.status_code,
			r.text.c_str());

		throw std::runtime_error{ "Received non 200 response from Steam API" };
	}

	const nl::json resp = nl::json::parse(r.text);

	const nl::json::json_pointer infoPath{ "/response/publishedfiledetails/0" };
	if (!resp.contains(infoPath))
	{
		qWarning(
			"Was not able to access the path '%s' in the response from Steam API, map ID: %s, "
			"response: %s",
			infoPath.to_string().c_str(),
			mapId.c_str(),
			resp.dump().c_str());

		throw std::runtime_error{ "Error while parsing Steam API response" };
	}

	const nl::json info = resp.at(infoPath);
	const std::string previewUrlField{ "preview_url" };
	if (!info.contains(previewUrlField))
	{
		qWarning(
			"Was not able to find the '%s' in the response from Steam API, map ID: %s, response: "
			"%s",
			previewUrlField.c_str(),
			mapId.c_str(),
			resp.dump().c_str());

		throw std::runtime_error{ "Error while parsing Steam API response" };
	}

	const std::string titleField{ "title" };
	if (!info.contains(previewUrlField))
	{
		qWarning(
			"Was not able to find the '%s' in the response from Steam API, map ID: %s, response: "
			"%s",
			titleField.c_str(),
			mapId.c_str(),
			resp.dump().c_str());

		throw std::runtime_error{ "Error while parsing Steam API response" };
	}
	const std::string mapName{ info.at(titleField) };
	const std::string previewUrl{ info.at(previewUrlField) };

	return { mapName, previewUrl };
}

void MapHistory::ReloadFile()
{
	emit ResetHistory();
	emit EntriesAdded(m_db.Select(m_historyFilters));
}

void MapHistory::ReloadFileWithFilters(
	const bool sortByRating, const bool removeDuplicated, const bool showOnlyBookmarks)
{
	m_historyFilters.m_sortByRating = sortByRating;
	m_historyFilters.m_removeDuplicated = removeDuplicated;
	m_historyFilters.m_showOnlyBookmarks = showOnlyBookmarks;
	ReloadFile();
}
