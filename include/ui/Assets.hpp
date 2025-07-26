#pragma once

#include <QCoreApplication>
#include <QString>

// For bundled app
// #define RESOURCES_PATH             (QCoreApplication::applicationDirPath() + "/../Resources/assets/")
// #define ICONS_PATH                 (RESOURCES_PATH + "icons/")

// For dev
#define RESOURCES_PATH             "../assets/"
#define ICONS_PATH                 RESOURCES_PATH "icons/"

#define CROSS_ICON                 QStringLiteral(ICONS_PATH "Cross.png")
#define LEFT_ICON                  QStringLiteral(ICONS_PATH "Left.png")
#define RIGHT_ICON                 QStringLiteral(ICONS_PATH "Right.png")
