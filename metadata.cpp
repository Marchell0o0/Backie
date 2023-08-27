#include "metadata.h"

#include <fstream>
#include <optional>
#include <map>

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

namespace fs = std::filesystem;

// Convert FileMetadata to JSON
static nlohmann::json toJson(FileMetadata meta){
    return {
        {"modificationTimestamp", meta.modificationTimestamp},
        {"fileSize", meta.fileSize},
        {"fileHash", meta.fileHash},
        {"isDeleted", meta.isDeleted}
    };
}

// Fetch FileMetadata from JSON
static FileMetadata fromJson(const nlohmann::json& j) {
    FileMetadata metadata;
    metadata.modificationTimestamp = j.at("modificationTimestamp").get<std::time_t>();
    metadata.fileSize = j.at("fileSize").get<uintmax_t>();
    metadata.fileHash = j.at("fileHash").get<std::string>();
    metadata.isDeleted = j.at("isDeleted").get<bool>();
    return metadata;
}

std::optional<std::map<fs::path, FileMetadata>> loadCombinedMdMap(const fs::path& jsonDestination) {
    // Fetch all backup directories and sort them based on creation date (assuming your naming convention allows this)
    std::vector<fs::directory_entry> backupDirs;
    for (const auto& dirEntry : fs::directory_iterator(jsonDestination)) {
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

FileMetadata MdFromFile(const fs::path& file){
    FileMetadata meta;
    meta.modificationTimestamp = fs::last_write_time(file).time_since_epoch().count();
    meta.fileSize = fs::file_size(file);
    meta.fileHash = std::to_string(meta.modificationTimestamp);  // TODO: Pseudo-hash for this example
    meta.isDeleted = false;
    return meta;
}

bool saveMdMap(const std::map<fs::path, FileMetadata>& metadata, const fs::path& jsonDestination) {
    nlohmann::json j;

    for (const auto& [path, meta] : metadata) {
        j[path.string()] = toJson(meta);
    }

    // Write the JSON to a file
    std::ofstream outFile(jsonDestination);
    if (!outFile) {
        SPDLOG_ERROR("Failed to open metadata file for writing");
        return false;
    }

    outFile << j.dump(4);
    return true;
}

std::map<fs::path, FileMetadata> loadMdMap(const fs::path& jsonDestination) {
    nlohmann::json j;
    std::ifstream inFile(jsonDestination);

    if (!inFile) {
        throw std::runtime_error("Failed to open metadata file for reading");
    }

    inFile >> j;

    std::map<fs::path, FileMetadata> metadata;
    for (const auto& item : j.items()) {
        metadata[fs::path(item.key())] = fromJson(item.value());
    }

    return metadata;
}
