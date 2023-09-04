#include "backuperrand.h"

#include <optional>
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <map>

#include "settings.h"
#include "spdlog/spdlog.h"
#include "utils.h"
#include "metadata.h"

std::string Backup::getName()  {
    return this->name;
}

BackupType Backup::getCurrentType() const {
    return this->currentType;
}

std::vector<std::string> Backup::getDestinations() const {
    return this->destinations;
}
std::vector<fs::path> Backup::getSources() const {
    return this->sources;
}

std::vector<std::shared_ptr<BackupSchedule>> Backup::getSchedules() const {
    return this->schedules;
}

std::string Backup::formatBackupFolderName(){
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm buf;
    localtime_s(&buf, &now_time); // TODO: Use "localtime_r" for non-Windows platforms
    std::ostringstream oss;
    oss << std::put_time(&buf, "[%d-%m-%Y %H.%M]");

    return strFromType(this->currentType) + " " + oss.str();
}

bool Backup::performBackup() {
    if (this->currentType == BackupType::FULL) {
        return fullBackup();
    } else if (this->currentType == BackupType::INCREMENTAL) {
        return incrementalBackup();
    } else {
        SPDLOG_ERROR("No current type specified");
        return false;
    }
}

bool Backup::backup(const fs::path file, const fs::path backupFolder, const fs::path sourcePath) {
    fs::path relativePath = fs::relative(file, sourcePath);
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

//TODO: Check if the destination exists
bool Backup::fullBackup() {
    Settings& settings = Settings::getInstance();
    for (const auto& destinationName : this->destinations) {
        fs::path backupFolder = settings.getDest(destinationName).destinationFolder / this->name / formatBackupFolderName();

        if (fs::exists(backupFolder)){
            SPDLOG_WARN("Backup folder with name: {}, already exists", backupFolder.u8string());
            return false;
        } else {
            fs::create_directory(backupFolder);
        }

        std::map<fs::path, FileMetadata> metadata;
        for (const auto& source : this->sources){
            fs::create_directory(backupFolder / source.string());
            for (const auto& file : fs::recursive_directory_iterator(source)) {
                if (file.is_regular_file()) {
                    if (backup(file.path(), backupFolder, source)){
                        metadata[file.path()] = MdFromFile(file.path());
                    } else {
                        return false;
                    }
                }
            }
        }
        if (!saveMdMap(metadata, backupFolder / "metadata.json")){
            return false;
        }
    }
    return true;
}

bool Backup::incrementalBackup() {
    Settings& settings = Settings::getInstance();
    for (const auto& destinationName : this->destinations){
        // Combined data from the last FULL backup and all subsequent backups
        fs::path destFolder = settings.getDest(destinationName).destinationFolder;
        std::optional<std::map<fs::path, FileMetadata>> combinedMetadata = loadCombinedMdMap(destFolder);
        if (!combinedMetadata){
            return false;
        }


        fs::path backupFolder = destFolder / this->name / formatBackupFolderName();
        if (fs::exists(backupFolder)){
            SPDLOG_WARN("Backup folder with name: {}, already exists", backupFolder.u8string());
            return false;
        } else {
            fs::create_directory(backupFolder);
        }

        // Backup changed files
        std::map<fs::path, FileMetadata> newMetadata;
        for (const auto& source : this->sources){
            fs::create_directory(backupFolder / source.string());
            for (const auto& file : fs::recursive_directory_iterator(source)) {
                if (file.is_regular_file() && shouldBackup(file.path(), *combinedMetadata)) {
                    if (backup(file.path(), backupFolder, source)){
                        newMetadata[file.path()] = MdFromFile(file.path());
                    } else {
                        return false;
                    }
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

        if (!saveMdMap(newMetadata, backupFolder / "metadata.json")){
            return false;
        }
    }

    return true;
}





