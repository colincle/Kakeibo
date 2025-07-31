#include "Backups.hpp"
#include "EnveloppeManager.hpp"
#include "Globals.hpp"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDate>
#include <QFileInfoList>
#include <algorithm>

#include <filesystem>

QString Backups::getPath()
{
	QString basePath = QString::fromStdString(std::filesystem::path(g_enveloppeManager.getPath()).string());
	QString jsonPath = basePath + "/backups.json";

	if ( !QFile::exists(jsonPath) )
	{
		QFile newFile(jsonPath);
		if ( newFile.open(QIODevice::WriteOnly) )
		{
			QJsonObject obj;
			obj["path"] = "";
			newFile.write(QJsonDocument(obj).toJson());
			newFile.close();
		}
	}

	QFile file(jsonPath);
	if ( !file.open(QIODevice::ReadOnly) )
		return "";

	QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
	file.close();

	QString storedPath = doc.object().value("path").toString();
	if ( storedPath.isEmpty() || !QDir(storedPath).exists() )
	{
    QString chosenDir = QFileDialog::getExistingDirectory(nullptr, "Sélectionner le dossier de backup / バックアップ用フォルダを選択");
		if ( chosenDir.isEmpty() )
			return "";

		QJsonObject newObj;
		newObj["path"] = chosenDir;

		if ( file.open(QIODevice::WriteOnly | QIODevice::Truncate) )
		{
			file.write(QJsonDocument(newObj).toJson());
			file.close();
		}
		return chosenDir;
	}
	return storedPath;
}

void Backups::backup()
{
	QString backupRoot = getPath();
	if (backupRoot.isEmpty())
		return;

	QString basePath = QString::fromStdString(std::filesystem::path(g_enveloppeManager.getPath()).string());
	QString todayName = QDate::currentDate().toString("yyyy-MM-dd");
	QString todayBackupPath = backupRoot + "/" + todayName;

	if (QDir(todayBackupPath).exists())
		return;

	if (!copyRecursively(basePath, todayBackupPath))
		return;

	QDir backupDir(backupRoot);
	QFileInfoList allBackups = backupDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time | QDir::Reversed);

	if (allBackups.size() > 30)
	{
		for (int i = 0; i < allBackups.size() - 30; ++i)
			QDir(allBackups[i].absoluteFilePath()).removeRecursively();
	}
}

bool Backups::copyRecursively(const QString &srcPath, const QString &dstPath)
{
	QDir srcDir(srcPath);
	if (!srcDir.exists())
		return false;

	QDir dstDir(dstPath);
	if (!dstDir.exists() && !QDir().mkpath(dstPath))
		return false;

	for (const QFileInfo &item : srcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries))
	{
		QString srcItemPath = item.absoluteFilePath();
		QString dstItemPath = dstPath + "/" + item.fileName();

		if (item.isDir())
		{
			if (!copyRecursively(srcItemPath, dstItemPath))
				return false;
		}
		else
		{
			if (!QFile::copy(srcItemPath, dstItemPath))
				return false;
		}
	}
	return true;
}
