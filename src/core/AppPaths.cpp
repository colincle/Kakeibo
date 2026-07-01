#include "AppPaths.hpp"
#include "Globals.hpp"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>

#include <filesystem>

namespace AppPaths
{
QString configuredDir(const QString &jsonFileName, const QString &promptTitle)
{
	QString basePath = QString::fromStdString(std::filesystem::path(g_envelopeManager.getPath()).string());
	QString jsonPath = basePath + "/" + jsonFileName;

	QString storedPath;

	QFile file(jsonPath);

	if ( file.open(QIODevice::ReadOnly) )
	{
		QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
		storedPath        = doc.object().value("path").toString();
		file.close();
	}

	if ( !storedPath.isEmpty() && QDir(storedPath).exists() )
		return storedPath;

	QString chosenDir = QFileDialog::getExistingDirectory(nullptr, promptTitle);

	if ( chosenDir.isEmpty() )
		return "";

	if ( file.open(QIODevice::WriteOnly | QIODevice::Truncate) )
	{
		QJsonObject obj;
		obj["path"] = chosenDir;
		file.write(QJsonDocument(obj).toJson());
		file.close();
	}

	return chosenDir;
}
} // namespace AppPaths
