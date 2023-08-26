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
#include "nlohmann/json.hpp"

namespace fs = std::filesystem;

struct FileMetadata {
    std::time_t modificationTimestamp;
    uintmax_t fileSize;
    std::string fileHash;  // Could be md5, sha256, etc.
    bool isDeleted;


    // Convert FileMetadata to JSON
    nlohmann::json toJson() const {
        return {
            {"modificationTimestamp", modificationTimestamp},
            {"fileSize", fileSize},
            {"fileHash", fileHash},
            {"isDeleted", isDeleted}
        };
    }

    // Populate FileMetadata from JSON
    static FileMetadata fromJson(const nlohmann::json& j) {
        FileMetadata metadata;
        metadata.modificationTimestamp = j.at("modificationTimestamp").get<std::time_t>();
        metadata.fileSize = j.at("fileSize").get<uintmax_t>();
        metadata.fileHash = j.at("fileHash").get<std::string>();
        metadata.isDeleted = j.at("isDeleted").get<bool>();
        return metadata;
    }
};

class Backup {
    friend class BackupFactory;

private:
    Backup(BackupType type, fs::path dir);

public:



    void performBackup();

    static BackupType getTypeFromStr(const std::string& string);

    fs::path getDirectory() const;
    BackupType getType() const;
    std::string getTypeStr() const;
private:
    BackupType type;
    fs::path destination = "W:\\Backie backups\\Dest 1";
    fs::path directory;


    bool FullBackup();
    void IncrementalBackup();

};

#endif // BACKUP_H
