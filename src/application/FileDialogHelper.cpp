#include "FileDialogHelper.h"

#include <QFileDialog>

FileDialogHelper::FileDialogHelper(QObject* parent)
	: QObject{ parent }
{
}

QString FileDialogHelper::openFile(const QString& title, const QString& filter)
{
	return QFileDialog::getOpenFileName(nullptr, title, QString(), filter);
}
