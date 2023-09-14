#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QWidget>
#include <string>
#include <filesystem>
#include <random>

//enum class BackupType {
//    FULL,           ///< Represents a full backup.
//    INCREMENTAL,     ///< Represents an incremental backup only of the files that have changed since full backup.
//    NONE           ///< Represents a default value for the type
//    // DIFFERENTIAL  // Future extension
//};
namespace fs = std::filesystem;

//BackupType typeFromStr(const std::string& string);

//const std::string strFromType(const BackupType type);

std::wstring strToWStr(const std::string& s);

std::string generate_uuid_v4();

bool IsRunningAsAdmin();

void loadStyleSheet(const QString& stylePath, QWidget* widget);

#endif // UTILS_H
