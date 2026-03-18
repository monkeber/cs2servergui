#include "AppData.h"
#include "FileDialogHelper.h"

#include <nfd.hpp>

FileDialogHelper::FileDialogHelper(QObject* parent)
	: QObject{ parent }
{
}

QString FileDialogHelper::openFile()
{
	NFD::Guard guard;
	NFD::UniquePath outPath;

	const std::filesystem::path path{
		AppData::Instance().settings()->getExecutablePath().toStdString()
	};

	nfdresult_t result =
		NFD::OpenDialog(outPath, nullptr, 0, path.empty() ? nullptr : path.parent_path().c_str());
	if (result == NFD_OKAY)
	{
		return QString::fromUtf8(outPath.get());
	}

	return {};
}
