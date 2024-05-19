// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <AppData.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QScopedPointer>

#include "app_environment.h"

int main(int argc, char* argv[])
{
	set_qt_environment();

	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	qmlRegisterSingletonInstance(
		"cs2server", 1, 0, "ProcessHandler", AppData::Instance().serverProcess.get());
	qmlRegisterSingletonInstance("cs2server", 1, 0, "Settings", AppData::Instance().settings.get());

	const QUrl url(u"qrc:/qt/qml/Main/Main.qml"_qs);
	QObject::connect(
		&engine,
		&QQmlApplicationEngine::objectCreated,
		&app,
		[url](QObject* obj, const QUrl& objUrl) {
			if (!obj && url == objUrl)
				QCoreApplication::exit(-1);
		},
		Qt::QueuedConnection);

	engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
	engine.addImportPath(":/");

	engine.load(url);

	if (engine.rootObjects().isEmpty())
	{
		return -1;
	}

	return app.exec();
}
