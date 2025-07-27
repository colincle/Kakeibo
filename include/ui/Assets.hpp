#pragma once

#include <QCoreApplication>
#include <QString>

// Bundled app
#define ASSETS_PATH (QCoreApplication::applicationDirPath() + "/../Resources/assets/")
#define ICONS_PATH (ASSETS_PATH + "icons/")

#define CROSS_ICON (ICONS_PATH + "Cross.png")
#define LEFT_ICON (ICONS_PATH + "Left.png")
#define RIGHT_ICON (ICONS_PATH + "Right.png")
#define DOWN_ICON (ICONS_PATH + "Down.png")
#define TRANSFER_ICON (ICONS_PATH + "Transfer.png")
#define PLUS_ICON (ICONS_PATH + "Plus.png")
#define IMPORT_ICON (ICONS_PATH + "Import.png")
#define MODIFY_ICON (ICONS_PATH + "Modify.png")
