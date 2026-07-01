#include "ConnectToCloud.hpp"
#include "AppPaths.hpp"

#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QPainter>
#include <QPixmap>
#include <QWidget>
#include <QtConcurrent/QtConcurrent>

void ConnectToCloud::sendCardsToCloud(const QList<QWidget *> &cloudWidgets)
{
	QString destPath = AppPaths::configuredDir("cloudPath.json", "Sélectionner le dossier de synchronisation / 同期フォルダを選択");

	if ( destPath.isEmpty() )
		return;

	// Grab the widgets now, on the GUI thread, while they are guaranteed alive.
	// The page rebuild (showEnvelopes) deletes and recreates these cards, so we
	// must never hand raw widget pointers to the worker thread — only the
	// resulting pixmaps (value types) cross the thread boundary.
	QList<QPixmap> screenshots = takeScreenshots(cloudWidgets);

	static QMutex                mutex;
	static QList<QList<QPixmap>> queue;
	static bool                  running = false;

	QMutexLocker locker(&mutex);
	queue.append(screenshots);

	if ( running )
		return;

	running = true;

	[[maybe_unused]] auto future = QtConcurrent::run([destPath]()
	                                                 {
		while(true)
		{
			QList<QPixmap> task;
			{
				QMutexLocker innerLocker(&mutex);

				if(queue.isEmpty())
				{
					running = false;
					break;
				}

				task = queue.takeLast();
				queue.clear();
			}

			QPixmap image = assembleIntoGrid(task);
			image.save(destPath + "/Kakeibo.png");
		} });
}

QList<QPixmap> ConnectToCloud::takeScreenshots(const QList<QWidget *> &widgets)
{
	// Trim the card's drop-shadow padding so the tiled image sits flush.
	const int cropTop    = 80;
	const int cropBottom = 15;
	const int cropRight  = 10;

	QList<QPixmap> screenshots;

	// Must run on the GUI thread: QWidget::grab() renders the live widget.
	for ( QWidget *w : widgets )
	{
		QPixmap original = w->grab();

		int croppedWidth  = original.width() - cropRight;
		int croppedHeight = original.height() - cropTop - cropBottom;

		screenshots.append(original.copy(0, cropTop, croppedWidth, croppedHeight));
	}

	return screenshots;
}

QPixmap ConnectToCloud::assembleIntoGrid(const QList<QPixmap> &screenshots)
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