#pragma once

#include <QString>

class Backups
{
public:
	Backups() = delete;
	Backups(const Backups&) = delete;
	Backups& operator=(const Backups&) = delete;

	static QString getPath();
	static void backup();
    static bool copyRecursively(const QString &srcPath, const QString &dstPath);
};
