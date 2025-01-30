#pragma once

#include <QObject>
#include <QQmlEngine>

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
	void entryAdded(const MapHistoryEntry& entry);
	void resetHistory();

public slots:
	//! Removes map entries from the map history file.
	void RemoveMapEntries(const int rowIndex, const int count);

public:
	//! Adds a new entry into the history. Handles saving of the info, downloading the preview, etc.
	static void Add(const std::string& mapId);

private:
	//! Downloads an image preview of the map and saves to a file in a folder in the current working
	//! directory. Returns a path to the downloaded file.
	static std::filesystem::path DownloadPreview(const std::string& mapId, const std::string& url);
	//! Requests the map info from the Steam API. Returns the received JSON.
	static nl::json GetMapInfo(const std::string& mapId);
	//! Adds a new entry with the provided info to map history file. Creates the file if such file
	//! does not exist yet.
	static void SaveMapEntry(
		const std::string& mapId, const std::string& mapName, const std::string& previewPath);

	//! Clears and then initializes the state (i.e. reads the map history file if such exists) and
	//! stores the data.
	void ReloadFile();

private:
	MapHistoryModel m_model;
	QVariant m_modelRef;
};
