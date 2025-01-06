#include "Log.h"

#include <fstream>

#include <QString>

namespace
{
namespace details
{

const char* LogFilePath{ "log.txt" };

}	 // namespace details
}	 // namespace

QtMessageHandler originalHandler = nullptr;

void InitLogging()
{
	qSetMessagePattern("[%{time yyyy-MM-dd h:mm:ss.zzz} %{type}] %{file}:%{line} - %{message}");
}

void SetOriginalHandler(QtMessageHandler handler)
{
	originalHandler = handler;
}

Log::Log(QObject* parent)
	: QObject{ parent }
{
	connect(&LogHandler::Instance(), &LogHandler::newLogReceived, this, &Log::AddNewMessage);
}

void Log::AddNewMessage(const QString& newmsg)
{
	emit newLogMessageCaptured(newmsg);
}

LogHandler& LogHandler::Instance()
{
	static LogHandler instance;

	return instance;
}

void LogHandler::MessageOutput(
	QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	static std::ofstream logFile{ std::filesystem::path{ details::LogFilePath },
		std::ios::out | std::ios::app };

	const auto formattedMsg{ qFormatLogMessage(type, context, msg) };

	logFile << qPrintable(formattedMsg) << "\n";
	logFile.flush();

	emit LogHandler::Instance().newLogReceived(formattedMsg);

	if (originalHandler)
	{
		originalHandler(type, context, msg);
	}
}
