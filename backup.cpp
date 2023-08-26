#include "backup.h"
#include "spdlog/spdlog.h"

#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <map>


Backup::Backup(BackupType type, std::filesystem::path dir)
    : type(type), directory(dir) {
}




std::filesystem::path Backup::getDirectory() const  {
    return directory;
}

BackupType Backup::getType() const {
    return type;
}

BackupType Backup::getTypeFromStr(const std::string& string) {
    BackupType type = BackupType::NONE;
    if (string == "FULL"){
        type = BackupType::FULL;
    } else if (string == "INCREMENTAL") {
        type = BackupType::INCREMENTAL;
        // other types
    }
    return type;
}

void Backup::performBackup() {
    if (type == BackupType::FULL) {
        FullBackup();
    } else if (type == BackupType::INCREMENTAL) {
        IncrementalBackup();
    }
}

bool saveMetadata(const std::map<fs::path, FileMetadata>& metadata, const fs::path& destination) {
    nlohmann::json j;

    for (const auto& [path, meta] : metadata) {
        j[path.string()] = meta.toJson();
    }

    // Write the JSON to a file
    std::ofstream outFile(destination);
    if (!outFile) {
        SPDLOG_ERROR("Failed to open metadata file for writing");
        return false;
    }

    outFile << j.dump(4);  // '4' specifies 4 spaces for indentation
    return true;
}

std::map<fs::path, FileMetadata> loadMetadata(const fs::path& source) {
    nlohmann::json j;
    std::ifstream inFile(source);

    if (!inFile) {
        throw std::runtime_error("Failed to open metadata file for reading");
    }

    inFile >> j;

    std::map<fs::path, FileMetadata> metadata;
    for (const auto& item : j.items()) {
        metadata[fs::path(item.key())] = FileMetadata::fromJson(item.value());
    }

    return metadata;
}


std::string Backup::getTypeStr() const{
    switch (type) {
    case BackupType::FULL:
        return "FULL";
    case BackupType::INCREMENTAL:
        return "INCREMENTAL";
    default:
        return "";
    }
}

bool Backup::FullBackup() {
    std::map<fs::path, FileMetadata> metadata;

    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Format time as "YY.MM.dd hh:mm"
    std::tm buf;
    localtime_s(&buf, &now_time); // Use "localtime_r" for non-Windows platforms
    std::ostringstream oss;
    oss << std::put_time(&buf, "%y.%m.%d");

    fs::path backupDest = destination / (getTypeStr() + " " + oss.str());
    fs::create_directory(backupDest);

    for (const auto& file : fs::recursive_directory_iterator(directory)) {
        if (file.is_regular_file()) {
            // Prepare metadata for the file
            FileMetadata meta;
            meta.modificationTimestamp = fs::last_write_time(file.path()).time_since_epoch().count();
            meta.fileSize = fs::file_size(file.path());
            meta.fileHash = std::to_string(meta.modificationTimestamp);  // Pseudo-hash for this example
            meta.isDeleted = false;

            // Store metadata
            metadata[file.path()] = meta;

            // Copy file to destination, preserving relative path
            fs::path relativePath = fs::relative(file.path(), directory);
            fs::path destPath = backupDest / relativePath;
//            fs::create_directories(destPath.parent_path());  // Ensure directories exist
            fs::copy(file.path(), destPath, fs::copy_options::overwrite_existing);
        }
    }

    return saveMetadata(metadata, backupDest / "metadata.json");
}

void Backup::IncrementalBackup() {
//    for (const auto& file : fs::recursive_directory_iterator(directory)) {
//        auto currentMetadata = fetchMetadata(file.path());
//        if (shouldBackup(file.path(), currentMetadata)) {
//            // Do backup
//            previousMetadata[file.path()] = currentMetadata;
//        }
//    }
//    saveMetadata();
}

//bool shouldBackup(const fs::path& path, const FileMetadata& currentMetadata) {
//    auto it = previousMetadata.find(path);
//    if (it == previousMetadata.end()) {
//        return true;  // New file
//    }
//    // Compare currentMetadata with it->second (i.e., previous metadata)
//    // and determine if the file should be backed up
//    return currentMetadata != it->second;
//}


