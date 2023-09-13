#include "metadata.h"

#include <fstream>
#include <optional>
#include <map>
#include <windows.h>

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

namespace fs = std::filesystem;

static nlohmann::json toJson(FileMetadata meta){
    return {
        {"modificationTimestamp", meta.modificationTimestamp},
        {"fileSize", meta.fileSize},
        {"fileHash", meta.fileHash},
        {"isDeleted", meta.isDeleted}
    };
}

static FileMetadata fromJson(const nlohmann::json& j) {
    FileMetadata metadata;
    metadata.modificationTimestamp = j.at("modificationTimestamp").get<std::time_t>();
    metadata.fileSize = j.at("fileSize").get<uintmax_t>();
    metadata.fileHash = j.at("fileHash").get<std::string>();
    metadata.isDeleted = j.at("isDeleted").get<bool>();
    return metadata;
}


//TODO: load by parent backups until it's a full one
std::optional<std::map<fs::path, FileMetadata>> Metadata::loadCombinedMdMap(const fs::path& destination, const std::string& name) {
    std::vector<fs::directory_entry> backupDirs;
    for (const auto& dirEntry : fs::directory_iterator(destination / name)) {
        if (fs::is_directory(dirEntry)) {
            backupDirs.push_back(dirEntry);
        }
    }

    std::sort(backupDirs.begin(), backupDirs.end(),
              [](const fs::directory_entry& a, const fs::directory_entry& b) {
                  return fs::last_write_time(a) < fs::last_write_time(b);
              });

    // Identify the last FULL backup and mark its position
    int lastFullBackupPos = -1;
    for (int i = 0; i < backupDirs.size(); ++i) {
        if (backupDirs[i].path().filename().string().find("FULL") != std::string::npos) {
            lastFullBackupPos = i;
            break;
        }
    }

    if (lastFullBackupPos == -1) {
        SPDLOG_ERROR("No full backup found");
        return std::nullopt;
    }

    std::map<fs::path, FileMetadata> combinedMetadata;

    // Load the FULL backup metadata first
    if (fs::exists(backupDirs[lastFullBackupPos].path() / "metadata.json")) {
        combinedMetadata = loadMdMap(backupDirs[lastFullBackupPos].path() / "metadata.json");
    } else {
        SPDLOG_ERROR("FULL backup doesn't have a metadata file");
        return std::nullopt;
    }

    // Iterate through each subsequent backup and combine the metadata
    for (int i = lastFullBackupPos + 1; i < backupDirs.size(); ++i) {
        std::map<fs::path, FileMetadata> currentBackupMetadata;
        if (fs::exists(backupDirs[i].path() / "metadata.json")) {
            currentBackupMetadata = loadMdMap(backupDirs[i].path() / "metadata.json");
            for (const auto& [path, meta] : currentBackupMetadata) {
                combinedMetadata[path] = meta; // This will overwrite old metadata with new if the path is the same
            }
        } else {
            SPDLOG_WARN("Backup in folder '{}' doesn't have a metadata file", backupDirs[i].path().string());
        }
    }
    return combinedMetadata;
}

void Metadata::addFile(const fs::path& file) {
    map[file] = MdFromFile(file);
}

void Metadata::initAllFiles(const std::vector<fs::path>& sources) {
    for (const auto& source : sources) {
        for (const auto& file : fs::recursive_directory_iterator(source)) {
            if (file.is_regular_file()) {
                addFile(file.path());
            }
        }
    }
}

void Metadata::initJson(const std::string& Id, const std::string& parentId) {
    for (const auto& [path, meta] : map) {
        MdJson[path.string()] = toJson(meta);
    }
    MdJson["parentID"] = parentId;
    MdJson["ID"] = Id;
}

void Metadata::initChangedFiles(const std::vector<fs::path>& sources,
                                const fs::path& destination,
                                const std::string& name) {
    this->combinedOldMap = *loadCombinedMdMap(destination, name);

    // Check for deleted files
    for (const auto& [path, meta] : this->combinedOldMap) {
        if (!fs::exists(path) && !meta.isDeleted) {
            FileMetadata deletedMeta = meta;
            deletedMeta.isDeleted = true;
            this->map[path] = deletedMeta;
        }
    }
}

FileMetadata Metadata::MdFromFile(const fs::path& file){
    FileMetadata meta;
    meta.modificationTimestamp = fs::last_write_time(file).time_since_epoch().count();
    meta.fileSize = fs::file_size(file);
    meta.fileHash = std::to_string(meta.modificationTimestamp);  // TODO: Pseudo-hash for this example
    meta.isDeleted = false;
    return meta;
}

bool Metadata::changed(const fs::path& file) {
    auto it = this->combinedOldMap.find(file);

    if (it == this->combinedOldMap.end()) {
        return true;
    } else {
        const FileMetadata& oldMeta = it->second;

        FileMetadata tempMeta;
        tempMeta.modificationTimestamp = fs::last_write_time(file).time_since_epoch().count();
        tempMeta.fileSize = fs::file_size(file);
        tempMeta.fileHash = std::to_string(tempMeta.modificationTimestamp); // TODO: Pseudo-hash
        tempMeta.isDeleted = false;

        if (tempMeta.modificationTimestamp != oldMeta.modificationTimestamp ||
            tempMeta.fileSize != oldMeta.fileSize ||
            tempMeta.fileHash != oldMeta.fileHash ||
            tempMeta.isDeleted != oldMeta.isDeleted) {
            return true;
        }
    }

    return false;
}

bool Metadata::save(const fs::path& jsonDestination) {
    std::ofstream outFile(jsonDestination);
    if (!outFile) {
        SPDLOG_ERROR("Failed to open metadata file for writing");
        return false;
    }

    if (MdJson.empty()) {
        SPDLOG_WARN("Json that is saved is empty");
    }

    outFile << this->MdJson.dump(4);
    outFile.close();
    //TODO: Add other attributes
    SetFileAttributes(jsonDestination.c_str(), GetFileAttributes(jsonDestination.c_str()) | FILE_ATTRIBUTE_HIDDEN);
    return true;
}

std::map<fs::path, FileMetadata> Metadata::loadMdMap(const fs::path& jsonDestination) {
    nlohmann::json j;
    std::ifstream inFile(jsonDestination);

    if (!inFile) {
        throw std::runtime_error("Failed to open metadata file for reading");
    }

    inFile >> j;

    std::map<fs::path, FileMetadata> metadata;
    for (const auto& item : j.items()) {
        if (item.key() == "parentID") {
            continue;
        } else if (item.key() == "ID") {
            continue;
        } else {
            metadata[fs::path(item.key())] = fromJson(item.value());
        }
    }

    return metadata;
}
