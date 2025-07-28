#pragma once

#include <QCoreApplication>
#include <QString>

// Bundled app
static QString ICONS_PATH = QCoreApplication::applicationDirPath() + "/../Resources/assets/icons/";

// Developpement
// static QString ICONS_PATH = "../assets/icons/";

#define CROSS_ICON (ICONS_PATH + "Cross.png")
#define LEFT_ICON (ICONS_PATH + "Left.png")
#define RIGHT_ICON (ICONS_PATH + "Right.png")
#define DOWN_ICON (ICONS_PATH + "Down.png")
#define TRANSFER_ICON (ICONS_PATH + "Transfer.png")
#define PLUS_ICON (ICONS_PATH + "Plus.png")
#define IMPORT_ICON (ICONS_PATH + "Import.png")
#define MODIFY_ICON (ICONS_PATH + "Modify.png")
#define CLOUD_WHITE_ICON (ICONS_PATH + "CloudWhite.png")
#define CLOUD_BLUE_ICON (ICONS_PATH + "CloudBlue.png")
