#pragma once

#include <QObject>
#include <QQmlEngine>

#include <string>

class MapHistory : public QObject
{
	Q_OBJECT
	QML_ELEMENT

public:
	MapHistory(QObject* parent = nullptr);

public:
	//! Returns history data.
	Q_INVOKABLE QList<QVariantMap> get() const;
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

	//! Initializes the state (i.e. reads the map history file if such exists) and stores the data.
	void Init();

private:
	QList<QVariantMap> m_history;
};
