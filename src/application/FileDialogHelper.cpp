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

	// MSVC Complained about being unable to cast from path's value type into nfd's type.
	const std::string parentPathTemp{ path.parent_path().string() };
	const nfdresult_t result{ NFD::OpenDialog(
		outPath, nullptr, 0, path.empty() ? nullptr : parentPathTemp.c_str()) };
	if (result == NFD_OKAY)
	{
		return QString::fromUtf8(outPath.get());
	}

	return {};
}
