#include "Settings.h"

#include <QDir>

Settings::Settings(QObject* parent)
	: QObject{ parent }
{
}

QString Settings::getExecutablePath() const
{
	return m_executablePath;
}

QString Settings::GetStartParameters() const
{
	return m_startParameters;
}

void Settings::setExecutablePath(QString fileUrl)
{
	const QUrl url{ fileUrl };
	m_executablePath = QDir::toNativeSeparators(url.toLocalFile());

	emit executablePathChanged(m_executablePath);
}
