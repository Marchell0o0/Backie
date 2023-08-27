#include "backup.h"

#include <optional>
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <map>

#include "spdlog/spdlog.h"
#include "utils.h"
#include "metadata.h"

Backup::Backup(BackupType type, std::filesystem::path dir)
    : type(type), directory(dir) {
}

std::filesystem::path Backup::getDirectory() const  {
    return directory;
}
BackupType Backup::getType() const {
    return type;
}

std::string Backup::formatBackupFolderName(){
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm buf;
    localtime_s(&buf, &now_time); // TODO: Use "localtime_r" for non-Windows platforms
    std::ostringstream oss;
    oss << std::put_time(&buf, "[%d-%m-%Y %H.%M]");

    return strFromType(type) + " " + oss.str();
}

bool Backup::performBackup() {
    if (type == BackupType::FULL) {
        return fullBackup();
    } else if (type == BackupType::INCREMENTAL) {
        return incrementalBackup();
    } else {
        return false;
    }
}

bool Backup::backup(const fs::path file, const fs::path backupFolder) {
    fs::path relativePath = fs::relative(file, directory);
    fs::path destPath = backupFolder / relativePath;
    fs::create_directories(destPath.parent_path());

    try {
        fs::copy(file, destPath);
    } catch (const fs::filesystem_error& e) {
        SPDLOG_ERROR("Couldn't copy with this error: {}", e.what());
        return false;
    }
    return true;
}


bool Backup::fullBackup() {
    fs::path backupFolder = globalDestination / formatBackupFolderName();

    if (fs::exists(backupFolder)){
        SPDLOG_WARN("Backup folder with name: {}, already exists", backupFolder.u8string());
        return false;
    } else {
        fs::create_directory(backupFolder);
    }

    std::map<fs::path, FileMetadata> metadata;
    for (const auto& file : fs::recursive_directory_iterator(directory)) {
        if (file.is_regular_file()) {
            if (backup(file.path(), backupFolder)){
                metadata[file.path()] = MdFromFile(file.path());
            } else {
                return false;
            }
        }
    }

    return saveMdMap(metadata, backupFolder / "metadata.json");
}

static bool shouldBackup(const fs::path& file, const std::map<fs::path, FileMetadata>& oldMetadata) {
    auto it = oldMetadata.find(file);

    // File didn't exist when the last full backup happened
    if (it == oldMetadata.end()) {
        return true;
    } else {
        const FileMetadata& oldMeta = it->second;

        FileMetadata tempMeta;
        tempMeta.modificationTimestamp = fs::last_write_time(file).time_since_epoch().count();
        tempMeta.fileSize = fs::file_size(file);
        tempMeta.fileHash = std::to_string(tempMeta.modificationTimestamp); // TODO: Pseudo-hash
        tempMeta.isDeleted = false;

        // Compare the fields of tempMeta and oldMeta
        if (tempMeta.modificationTimestamp != oldMeta.modificationTimestamp ||
            tempMeta.fileSize != oldMeta.fileSize ||
            tempMeta.fileHash != oldMeta.fileHash ||
            tempMeta.isDeleted != oldMeta.isDeleted) {
            return true;
        }
    }

    return false;
}



bool Backup::incrementalBackup() {
    // Combined data from the last FULL backup and all subsequent backups
    std::optional<std::map<fs::path, FileMetadata>> combinedMetadata = loadCombinedMdMap(globalDestination);
    if (!combinedMetadata){
        return false;
    }

    fs::path backupFolder = globalDestination / formatBackupFolderName();

    if (fs::exists(backupFolder)){
        SPDLOG_WARN("Backup folder with name: {}, already exists", backupFolder.u8string());
        return false;
    } else {
        fs::create_directory(backupFolder);
    }

    // Backup changed files
    std::map<fs::path, FileMetadata> newMetadata;
    for (const auto& file : fs::recursive_directory_iterator(directory)) {
        if (file.is_regular_file() && shouldBackup(file.path(), *combinedMetadata)) {
            if (backup(file.path(), backupFolder)){
                newMetadata[file.path()] = MdFromFile(file.path());
            } else {
                return false;
            }
        }
    }

    // Check for deleted files
    for (const auto& [path, meta] : *combinedMetadata) {
        if (!fs::exists(path) && !meta.isDeleted) {
            FileMetadata deletedMeta = meta;
            deletedMeta.isDeleted = true;
            newMetadata[path] = deletedMeta;
        }
    }

    return saveMdMap(newMetadata, backupFolder / "metadata.json");
}





