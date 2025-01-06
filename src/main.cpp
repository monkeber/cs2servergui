#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QScopedPointer>

#include "app_environment.h"
#include "Log.h"

int main(int argc, char* argv[])
{
	set_qt_environment();

	InitLogging();

	SetOriginalHandler(qInstallMessageHandler(LogHandler::MessageOutput));

	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;

	const QUrl url(u"qrc:/qt/qml/ui/Main.qml"_qs);
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
