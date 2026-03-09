#pragma once

#include <QObject>
#include <QQmlEngine>

#include "MapHistoryDatabase.h"
#include "MapHistoryModel.h"

#include <string>

class MapHistory : public QObject
{
	Q_OBJECT
	QML_ELEMENT

	Q_PROPERTY(QVariant model MEMBER m_modelRef CONSTANT)

public:
	MapHistory(QObject* parent = nullptr);

signals:
	void EntriesAdded(const std::vector<MapHistoryEntry>& entries);
	void ResetHistory();

public slots:
	//! Removes map entries from the map history file.
	void RemoveMapEntries(const std::string& workshopId, const std::string& playedAt);
	//! Updates the bookmarked status of the specified map ID.
	void UpdateMapBookmarked(const std::string& workshopId, const bool isBookmarked);
	//! Updates the rating of the specified map ID.
	void UpdateMapRating(const std::string& workshopId, const std::uint8_t rating);

public:
	//! Adds a new entry into the history. Handles saving of the info, downloading the preview, etc.
	void Add(const std::string& mapId);

private:
	//! Downloads an image preview of the map and saves to a file in a folder in the current working
	//! directory. Returns a path to the downloaded file.
	static std::filesystem::path DownloadPreview(const std::string& mapId, const std::string& url);
	//! Requests the map info from the Steam API. Returns the received JSON.
	static std::pair<std::string, std::string> GetMapNameAndPreviewUrl(const std::string& mapId);

	//! Clears and then initializes the state (i.e. reads the map history file if such exists) and
	//! stores the data.
	void ReloadFile();

private slots:
	void ReloadFileWithFilters(const bool sortByRating, const bool removeDuplicated, const bool showOnlyBookmarks);

private:
	struct HistoryFilters
	{
		bool m_sortByRating{ false };
		bool m_removeDuplicated{ false };
		bool m_showOnlyBookmarks{ false };
	};
	HistoryFilters m_historyFilters;

	MapHistoryModel m_model;
	QVariant m_modelRef;
	//! Handler of the underlying database where map history is stored.
	MapHistoryDatabase m_db;
};
