#pragma once

#include <QCoreApplication>
#include <QString>

// Bundled app
// #define ASSETS_PATH     (QCoreApplication::applicationDirPath() +
// "/../Resources/assets/") #define ICONS_PATH      (ASSETS_PATH + "icons/")

// Dev
#define ASSETS_PATH "../assets/"
#define ICONS_PATH ASSETS_PATH "icons/"

#define CROSS_ICON QStringLiteral(ICONS_PATH "Cross.png")
#define LEFT_ICON QStringLiteral(ICONS_PATH "Left.png")
#define RIGHT_ICON QStringLiteral(ICONS_PATH "Right.png")
#define DOWN_ICON QStringLiteral(ICONS_PATH "Down.png")
#define TRANSFER_ICON QStringLiteral(ICONS_PATH "Transfer.png")
#define PLUS_ICON QStringLiteral(ICONS_PATH "Plus.png")
#define IMPORT_ICON QStringLiteral(ICONS_PATH "Import.png")
#define MODIFY_ICON QStringLiteral(ICONS_PATH "Modify.png")
