#pragma once

#include <QList>

class QWidget;
class QPixmap;

class ConnectToCloud
{
  public:
	static QString        getPath();
	static void           sendCardsToCloud(const QList<QWidget *> &cloudWidgets);
	static QList<QPixmap> takeScreenshots(const QList<QWidget *> &widgets);
	static QPixmap        assembleIntoIPhoneShape(const QList<QPixmap> &screenshots);

  private:
	QString path;

	ConnectToCloud()                                  = delete;
	~ConnectToCloud()                                 = delete;
	ConnectToCloud(const ConnectToCloud &)            = delete;
	ConnectToCloud &operator=(const ConnectToCloud &) = delete;
};
