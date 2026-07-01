#pragma once

#include <QString>

namespace AppPaths
{
// Returns a user-configured directory stored under the app data folder in
// <jsonFileName> (JSON key "path"). If it is missing or no longer exists,
// the user is asked to pick one with promptTitle and the choice is saved.
// Returns an empty string if the user cancels.
QString configuredDir(const QString &jsonFileName, const QString &promptTitle);
} // namespace AppPaths
