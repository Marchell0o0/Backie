/**
 * @file backup.h
 * @brief Backup Class
 *
 * This header file contains the definitions for managing and executing backups.
 */

#ifndef BACKUP_H
#define BACKUP_H

#include <filesystem>

#include "utils.h"

namespace fs = std::filesystem;

class Backup {
    friend class BackupFactory;

private:
    Backup(BackupType type, fs::path dir);

public:

    fs::path destination = "W:\\backup_testing\\destination";

//    void performFullBackup() {
//        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
//            if (entry.is_regular_file()) {
//                auto destPath = destination / fs::relative(entry.path(), directory);
//                fs::create_directories(destPath.parent_path()); // Ensure destination directory exists
//                fs::copy(entry.path(), destPath, fs::copy_options::overwrite_existing);

//                // Update the metadata with the current file's timestamp
//                metadata.updateTimestamp(entry.path(), fs::last_write_time(entry.path()));
//            }
//        }
//    }

//    void performIncrementalBackup() {
//        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
//            if (entry.is_regular_file()) {
//                auto currentTimestamp = fs::last_write_time(entry.path());
//                if (metadata.hasChangedSinceLastBackup(entry.path(), currentTimestamp)) {
//                    auto destPath = destination / fs::relative(entry.path(), directory);
//                    fs::create_directories(destPath.parent_path()); // Ensure destination directory exists
//                    fs::copy(entry.path(), destPath, fs::copy_options::overwrite_existing);

//                    // Update the metadata with the current file's timestamp
//                    metadata.updateTimestamp(entry.path(), currentTimestamp);
//                }
//            }
//        }
//    }

    static BackupType getTypeFromStr(const std::string& string);

    fs::path getDirectory() const;
    BackupType getType() const;
private:
    BackupType type;
    fs::path directory;
};

#endif // BACKUP_H
