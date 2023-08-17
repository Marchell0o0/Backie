#ifndef UTILS_H
#define UTILS_H

#include <string>

/**
 * @enum BackupType
 * @brief Types of backups supported by Backie.
 */
enum class BackupType {
    NONE,           ///< Represents a default value for the type
    FULL,           ///< Represents a full backup.
    INCREMENTAL     ///< Represents an incremental backup only of the files that have changed since full backup.
    // DIFFERENTIAL  // Future extension
};

bool IsRunningAsAdmin();

std::wstring stringToWString(const std::string& s);

#endif // UTILS_H
