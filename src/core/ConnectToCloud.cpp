#include "ConnectToCloud.hpp"
#include "Globals.hpp"

#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QPainter>
#include <QPixmap>
#include <QStandardPaths>
#include <QtConcurrent/QtConcurrent>

#include <filesystem>
#include <iostream>

void ConnectToCloud::sendCardsToCloud(const QList<QWidget *> &cloudWidgets)
{
	static QMutex                  mutex;
	static QList<QList<QWidget *>> queue;
	static bool                    running = false;

	QString destPath = getPath();
	QMutexLocker locker(&mutex);
	queue.append(cloudWidgets);
	if (running)
		return;
	running = true;

	[[maybe_unused]] auto future = QtConcurrent::run([destPath, cloudWidgets]() mutable {
		while (true)
		{
			QList<QWidget *> task;
			{
				QMutexLocker innerLocker(&mutex);
				if (queue.isEmpty())
				{
					running = false;
					break;
				}
				task = queue.takeLast();
				queue.clear();
			}

			QList<QPixmap> screenshots = takeScreenshots(task);
			QPixmap image = assembleIntoIPhoneShape(screenshots);
			image.save(destPath + "/Kakeibo.png");
		}
	});
}

QString ConnectToCloud::getPath()
{
	QString basePath = QString::fromStdString(std::filesystem::path(g_enveloppeManager.getPath()).string());
	QString jsonPath = basePath + "/cloudPath.json";

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
		QString chosenDir = QFileDialog::getExistingDirectory(nullptr, "Sélectionner le dossier de synchronisation / 同期フォルダを選択");

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

QList<QPixmap> ConnectToCloud::takeScreenshots(const QList<QWidget *> &widgets)
{
	const int cropTop    = 80;
	const int cropBottom = 15;
	const int cropRight  = 10;

	QList<QPixmap> screenshots;

	for ( QWidget *w : widgets )
	{
        QPixmap original;
        QMetaObject::invokeMethod(QApplication::instance(), [&]() {
            original = w->grab();
        }, Qt::BlockingQueuedConnection);
		int     width    = original.width();
		int     height   = original.height();

		int x             = 0;
		int y             = cropTop;
		int croppedWidth  = width - cropRight;
		int croppedHeight = height - cropTop - cropBottom;

		screenshots.append(original.copy(x, y, croppedWidth, croppedHeight));
	}

	return screenshots;
}

QPixmap ConnectToCloud::assembleIntoIPhoneShape(const QList<QPixmap> &screenshots)
{
	if ( screenshots.isEmpty() )
		return QPixmap();

	const int    count     = static_cast<int>(screenshots.size());
	const int    separator = 4;
	const QColor separatorColor("#242F32");
	const int    columns = qCeil(qSqrt(count * 0.5));
	const int    rows    = qCeil(double(count) / columns);

	const int imgWidth  = screenshots.first().width();
	const int imgHeight = screenshots.first().height();

	const int totalWidth  = columns * imgWidth + (columns - 1) * separator;
	const int totalHeight = rows * imgHeight + (rows - 1) * separator;

	QPixmap finalImage(totalWidth, totalHeight);
	finalImage.fill(separatorColor);
	QPainter painter(&finalImage);
	painter.setRenderHint(QPainter::Antialiasing);

	for ( int i = 0; i < count; ++i )
	{
		int row = i / columns;
		int col = i % columns;
		int x   = col * (imgWidth + separator);
		int y   = row * (imgHeight + separator);
		painter.drawPixmap(x, y, screenshots[i]);
	}

	return finalImage;
}