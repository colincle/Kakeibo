#pragma once

#include <QCoreApplication>
#include <QString>

// Asset locations differ between a running-from-source build and a bundled
// macOS .app. Define KAKEIBO_DEV_ASSETS (e.g. -DKAKEIBO_DEV_ASSETS) to load
// icons from the source tree; otherwise they are read from the bundle.
#ifdef KAKEIBO_DEV_ASSETS
#define ASSETS_PATH QStringLiteral("../assets/")
#else
#define ASSETS_PATH (QCoreApplication::applicationDirPath() + "/../Resources/assets/")
#endif

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
#define UNDO_ICON (ICONS_PATH + QStringLiteral("Undo.png"))
#define REDO_ICON (ICONS_PATH + QStringLiteral("Redo.png"))
