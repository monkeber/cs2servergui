#include "MapHistory.h"
#include "AppData.h"

#include <QtLogging>

#include <cpr/cpr.h>
#include <rapidcsv.h>

namespace
{
namespace details
{

namespace columns
{

const char* MapWorkshopId{ "Map Workshop ID" };
const char* DownloadedAt{ "Downloaded At" };
const char* MapName{ "Map Name" };
const char* PreviewPath{ "Preview Path" };

}	 // namespace columns

//! Returns the path for map history file.
const std::filesystem::path& GetFilePath()
{
	static const std::filesystem::path filepath{ "map_history.csv" };
	return filepath;
}

//! Returns whether the file exists and is a regular file.
bool FileExists()
{
	return std::filesystem::exists(GetFilePath())
		&& std::filesystem::is_regular_file(GetFilePath());
}

//! Returns whether the file is empty.
bool IsFileEmpty()
{
	return std::filesystem::is_empty(GetFilePath());
}

//! Tries to open and return a file for writing or reading, if the file does not exist - will create
//! a new one, if it's empty - will set the columns, if the file exists and not empty - returns the
//! file. If the file is ill formed will return an empty Document object and false.
std::pair<rapidcsv::Document, bool> OpenForReadWrite()
{
	if (!FileExists())
	{
		std::ofstream file{ GetFilePath() };
		file.close();
	}

	if (IsFileEmpty())
	{
		rapidcsv::Document doc{ GetFilePath().string(), rapidcsv::LabelParams{ 0 } };
		doc.SetColumnName(0, columns::MapWorkshopId);
		doc.SetColumnName(1, columns::MapName);
		doc.SetColumnName(2, columns::DownloadedAt);
		doc.SetColumnName(3, columns::PreviewPath);

		return { doc, true };
	}

	rapidcsv::Document doc{ GetFilePath().string(), rapidcsv::LabelParams{ 0 } };
	if (doc.GetColumnCount() != 4)
	{
		qWarning("Map history file is ill-formed, cannot open: %s", GetFilePath().string().c_str());

		return { rapidcsv::Document{}, false };
	}

	return {
		rapidcsv::Document{ GetFilePath().string(), rapidcsv::LabelParams{ 0 } },
		true,
	};
}

}	 // namespace details
}	 // namespace

MapHistory::MapHistory(QObject* parent)
	: QObject{ parent }
{
	m_modelRef = QVariant::fromValue(&m_model);

	QObject::connect(this, &MapHistory::entryAdded, &m_model, &MapHistoryModel::AddEntry);
	QObject::connect(this, &MapHistory::resetHistory, &m_model, &MapHistoryModel::ClearModel);
	QObject::connect(
		&m_model, &MapHistoryModel::removeMapEntries, this, &MapHistory::RemoveMapEntries);
	ReloadFile();
}

void MapHistory::RemoveMapEntries(const int rowIndex, const int count)
{
	auto&& [doc, isOpen] = details::OpenForReadWrite();

	if (!isOpen)
	{
		return;
	}

	for (int i = 0; i < count; ++i)
	{
		doc.RemoveRow(rowIndex);
	}

	doc.Save();
}

void MapHistory::Add(const std::string& mapId)
{
	const nl::json resp = GetMapInfo(mapId);

	const nl::json::json_pointer infoPath{ "/response/publishedfiledetails/0" };
	if (!resp.contains(infoPath))
	{
		qWarning("Was not able to access the path '%s' in the response from Steam API, map ID: %s",
			infoPath.to_string().c_str(),
			mapId.c_str());

		return;
	}

	const nl::json info = resp.at(infoPath);

	const std::string previewUrlField{ "preview_url" };
	if (!info.contains(previewUrlField))
	{
		qWarning("Was not able to find the '%s' in the response from Steam API, map ID: %s",
			previewUrlField.c_str(),
			mapId.c_str());

		return;
	}
	const std::string previewUrl{ info.at(previewUrlField) };

	const std::string titleField{ "title" };
	if (!info.contains(previewUrlField))
	{
		qWarning("Was not able to find the '%s' in the response from Steam API, map ID: %s",
			titleField.c_str(),
			mapId.c_str());

		return;
	}
	const std::string mapName{ info.at(titleField) };

	const auto filePath{ DownloadPreview(mapId, previewUrl) };

	SaveMapEntry(mapId, mapName, filePath.relative_path().generic_string());

	AppData::Instance().mapHistory()->ReloadFile();
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
	auto&& [doc, isOpen] = details::OpenForReadWrite();
	if (!isOpen)
	{
		return;
	}

	const std::chrono::zoned_time currentTime{ std::chrono::current_zone(),
		std::chrono::system_clock::now() };

	doc.InsertRow<std::string>(doc.GetRowCount(),
		{
			mapId,
			mapName,
			std::format("{:%d-%m-%Y %H:%M:%OS}", currentTime.get_local_time()),
			previewPath,
		});

	doc.Save();
}

void MapHistory::ReloadFile()
{
	emit resetHistory();

	auto&& [doc, isOpen] = details::OpenForReadWrite();

	// If there were any errors during opening or the file does not have any entries yet.
	if (!isOpen || doc.GetRowCount() == 0)
	{
		return;
	}

	const auto getValue = [&doc](const std::string& columnName, const std::size_t rowNum) {
		try
		{
			return QString::fromStdString(doc.GetCell<std::string>(columnName, rowNum));
		}
		catch (const std::exception& e)
		{
			qWarning(
				"Can't retrieve value from map history file, column name: %s, row number: %d, "
				"error: %s",
				columnName.c_str(),
				rowNum,
				e.what());
		}
		catch (...)
		{
			qWarning("Can't retrieve value from map history file, column name: %s, row number: %d",
				columnName.c_str(),
				rowNum);
		}

		return QString{};
	};

	for (int i = 0; i < doc.GetRowCount(); ++i)
	{
		MapHistoryEntry entry;
		entry.m_workshopID = getValue(details::columns::MapWorkshopId, i);
		entry.m_mapName = getValue(details::columns::MapName, i);
		entry.m_downloadedAt = getValue(details::columns::DownloadedAt, i);

		const std::filesystem::path path{
			getValue(details::columns::PreviewPath, i).toStdString()
		};
		if (std::filesystem::exists(path))
		{
			entry.m_previewPath =
				QString{ "file:///%1" }.arg(std::filesystem::canonical(path).string().c_str());
		}

		emit entryAdded(entry);
	}
}
