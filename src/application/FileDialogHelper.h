#pragma once

#include <QObject>
#include <QQmlEngine>

class FileDialogHelper : public QObject
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

public:
	explicit FileDialogHelper(QObject* parent = nullptr);

	Q_INVOKABLE QString openFile();
};
