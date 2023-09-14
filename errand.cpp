#include "errand.h"

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

std::string Errand::getKey() const {
    return this->key;
}

BackupType Errand::getCurrentType() const {
    return this->currentType;
}

std::vector<std::string> Errand::getDestinations() const {
    return this->destinations;
}
std::vector<fs::path> Errand::getSources() const {
    return this->sources;
}
std::string Errand::getName() const {
    return this->name;
}

void Errand::setCurrentType(BackupType type) {
    this->currentType = type;
    return;
}

static std::string formatDirName(BackupType type){
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm buf;
    localtime_s(&buf, &now_time); // TODO: Use "localtime_r" for non-Windows platforms
    std::ostringstream oss;
    oss << std::put_time(&buf, "[%d-%m-%Y %H.%M]");

    return strFromType(type) + " " + oss.str();
}

static bool copy(const fs::path file, const fs::path backupFolder, const fs::path sourcePath) {
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


bool Errand::perform() {
    if (this->currentType == BackupType::NONE) {
        SPDLOG_ERROR("No current type specified");
        return false;
    }

    this->Id = generate_uuid_v4();
    Settings& settings = Settings::getInstance();
    settings.setLatestId(this->key, this->Id);

    switch (this->currentType) {
    case BackupType::FULL:
        return full();
    case BackupType::INCREMENTAL:
        return incremental();
    default:
        return false;
    }
}

//TODO: Count successful backups
bool Errand::full() {
    Metadata metadata;
    metadata.initAllFiles(this->sources);
    metadata.initJson(this->Id, this->parentId);

    Settings& settings = Settings::getInstance();
    for (const auto& destinationKey : this->destinations) {
        auto dest = settings.getDest(destinationKey);
        if (!dest) {
            SPDLOG_WARN("One of the destinations doesn't exist");
            continue;
        } else if (!fs::exists(dest->destinationFolder)) {
            SPDLOG_WARN("One of the destination folders doesn't exist");
            continue;
        }

        fs::path backupFolder = dest->destinationFolder / this->name / formatDirName(this->currentType);
        if (fs::exists(backupFolder)){
            SPDLOG_WARN("Backup folder with name: {}, already exists", backupFolder.u8string());
            continue;
        } else {
            fs::create_directories(backupFolder);
        }

        for (const auto& source : this->sources){
            fs::create_directory(backupFolder / source.string());
            for (const auto& file : fs::recursive_directory_iterator(source)) {
                if (file.is_regular_file()) {
                    copy(file.path(), backupFolder, source);
                }
            }
        }
        metadata.save(backupFolder / "metadata.json");
    }
    return true;
}

bool Errand::incremental() {
    Settings& settings = Settings::getInstance();
    for (const auto& destinationKey : this->destinations){
        auto dest = settings.getDest(destinationKey);
        if (!dest) {
            SPDLOG_WARN("One of the destinations doesn't exist");
            continue;
        } else if (!fs::exists(dest->destinationFolder)) {
            SPDLOG_WARN("One of the destination folders doesn't exist");
            continue;
        }

        Metadata metadata;
        metadata.initChangedFiles(this->sources, dest->destinationFolder, this->name);

        fs::path backupFolder = dest->destinationFolder / this->name / formatDirName(this->currentType);
        if (fs::exists(backupFolder)){
            SPDLOG_WARN("Backup folder with name: {}, already exists", backupFolder.u8string());
            continue;;
        } else {
            fs::create_directories(backupFolder);
        }

        // Backup changed files
        for (const auto& source : this->sources){
            fs::create_directory(backupFolder / source.string());
            for (const auto& file : fs::recursive_directory_iterator(source)) {
                if (file.is_regular_file() && metadata.changed(file.path())) {
                    copy(file.path(), backupFolder, source);
                    metadata.addFile(file.path());
                }
            }
        }

        //TODO: new metadata could be empty
        metadata.initJson(this->Id, this->parentId);
        metadata.save(backupFolder / "metadata.json");
    }
    return true;
}





