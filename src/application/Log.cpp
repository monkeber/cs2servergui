#include "Log.h"

#include <QString>

#include "AppData.h"

namespace
{
namespace details
{

const char* LogFilePath{ "log.txt" };

}	 // namespace details
}	 // namespace

QtMessageHandler originalHandler = nullptr;

FILE* InitFile()
{
	FILE* f = nullptr;
	fopen_s(&f, details::LogFilePath, "a");

	return f;
}

void InitLogging()
{
	qSetMessagePattern("[%{time yyyy-MM-dd h:mm:ss.zzz} %{type}] %{file}:%{line} - %{message}");
}

void MessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	static FILE* f = InitFile();

	const auto formattedMsg{ qFormatLogMessage(type, context, msg) };

	fprintf(f, "%s\n", qPrintable(formattedMsg));
	fflush(f);

	AppData::Instance().log()->AddNewMessage(formattedMsg);

	if (originalHandler)
	{
		originalHandler(type, context, msg);
	}
}

void SetOriginalHandler(QtMessageHandler handler)
{
	originalHandler = handler;
}

Log::Log(QObject* parent)
	: QObject{ parent }
{
}

void Log::AddNewMessage(const QString& newmsg)
{
	emit newLogMessageCaptured(newmsg);
}
