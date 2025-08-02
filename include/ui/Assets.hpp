#pragma once

#include <QCoreApplication>
#include <QString>

// Bundled app
#define ASSETS_PATH (QCoreApplication::applicationDirPath() + "/../Resources/assets/")
#define ICONS_PATH (ASSETS_PATH + "icons/")

#define CROSS_ICON (ICONS_PATH + QStringLiteral("Cross.png"))
#define LEFT_ICON (ICONS_PATH + QStringLiteral("Left.png"))
#define RIGHT_ICON (ICONS_PATH + QStringLiteral("Right.png"))
#define DOWN_ICON (ICONS_PATH + QStringLiteral("Down.png"))
#define TRANSFER_ICON (ICONS_PATH + QStringLiteral("Transfer.png"))
#define PLUS_ICON (ICONS_PATH + QStringLiteral("Plus.png"))
#define IMPORT_ICON (ICONS_PATH + QStringLiteral("Import.png"))
#define MODIFY_ICON (ICONS_PATH + QStringLiteral("Modify.png"))
#define CLOUD_WHITE_ICON (ICONS_PATH + QStringLiteral("CloudWhite.png"))
#define CLOUD_BLUE_ICON (ICONS_PATH + QStringLiteral("CloudBlue.png"))
#define LOCKED_ICON (ICONS_PATH + QStringLiteral("Locked.png"))
#define UNLOCKED_ICON (ICONS_PATH + QStringLiteral("Unlocked.png"))
#define INCREMENT_ICON (ICONS_PATH + QStringLiteral("Increment.png"))
#define DECREMENT_ICON (ICONS_PATH + QStringLiteral("Decrement.png"))

// Developpement
// static QString ICONS_PATH = "../assets/icons/";

// #define CROSS_ICON (ICONS_PATH + "Cross.png")
// #define LEFT_ICON (ICONS_PATH + "Left.png")
// #define RIGHT_ICON (ICONS_PATH + "Right.png")
// #define DOWN_ICON (ICONS_PATH + "Down.png")
// #define TRANSFER_ICON (ICONS_PATH + "Transfer.png")
// #define PLUS_ICON (ICONS_PATH + "Plus.png")
// #define IMPORT_ICON (ICONS_PATH + "Import.png")
// #define MODIFY_ICON (ICONS_PATH + "Modify.png")
// #define CLOUD_WHITE_ICON (ICONS_PATH + "CloudWhite.png")
// #define CLOUD_BLUE_ICON (ICONS_PATH + "CloudBlue.png")
// #define LOCKED_ICON (ICONS_PATH + "Locked.png")
// #define UNLOCKED_ICON (ICONS_PATH + "Unlocked.png")
// #define INCREMENT_ICON (ICONS_PATH + "Increment.png")
// #define DECREMENT_ICON (ICONS_PATH + "Decrement.png")
