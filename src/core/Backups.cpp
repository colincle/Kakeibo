#include "Backups.hpp"
#include "AppPaths.hpp"
#include "Globals.hpp"

#include <QDate>
#include <QDir>
#include <QFile>
#include <QFileInfoList>

#include <filesystem>

void Backups::backup()
{
	QString backupRoot = AppPaths::configuredDir("backups.json", "Sélectionner le dossier de backup / バックアップ用フォルダを選択");

	if ( backupRoot.isEmpty() )
		return;

	QString basePath        = QString::fromStdString(std::filesystem::path(g_envelopeManager.getPath()).string());
	QString todayName       = QDate::currentDate().toString("yyyy-MM-dd");
	QString todayBackupPath = backupRoot + "/" + todayName;

	if ( QDir(todayBackupPath).exists() )
		return;

	if ( !copyRecursively(basePath, todayBackupPath) )
		return;

	QDir          backupDir(backupRoot);
	QFileInfoList allBackups = backupDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time | QDir::Reversed);

	if ( allBackups.size() > 30 )
	{
		for ( int i = 0; i < allBackups.size() - 30; ++i )
			QDir(allBackups[i].absoluteFilePath()).removeRecursively();
	}
}

bool Backups::copyRecursively(const QString &srcPath, const QString &dstPath)
{
	QDir srcDir(srcPath);

	if ( !srcDir.exists() )
		return false;

	QDir dstDir(dstPath);

	if ( !dstDir.exists() && !QDir().mkpath(dstPath) )
		return false;

	for ( const QFileInfo &item : srcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries) )
	{
		QString srcItemPath = item.absoluteFilePath();
		QString dstItemPath = dstPath + "/" + item.fileName();

		if ( item.isDir() )
		{
			if ( !copyRecursively(srcItemPath, dstItemPath) )
				return false;
		}
		else
		{
			if ( !QFile::copy(srcItemPath, dstItemPath) )
				return false;
		}
	}

	return true;
}
