#include <windows.h>
#include <ShlObj.h>
#include <vector>

#include "spdlog/spdlog.h"

#include "backupbuilder.h"
#include "settings.h"

namespace fs = std::filesystem;

bool Settings::initializeSettings() {
    char pathToAppData[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, pathToAppData))) {
        std::string backieFolderPath = std::string(pathToAppData) + "\\Backie";
        if (GetFileAttributesA(backieFolderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
            CreateDirectoryA(backieFolderPath.c_str(), NULL);
        }
        path = backieFolderPath + "\\settings.json";
    } else {
        // TODO: Couldn't find appdata (crossplatform)
        SPDLOG_ERROR("Couldn't find appdata");
        return false;
    }
    SPDLOG_INFO("Path to settings {}", path.string());

    std::ifstream file(path);
    if (!file.is_open()) {
        SPDLOG_WARN("Creating new settings file");
        std::ofstream fout(path);
        if (fout.is_open()) {
            fout << data.dump(4);
            fout.close();
        } else {
            SPDLOG_ERROR("Could not create new settings file");
            return false;
        }
    } else {
        try {
            data = json::parse(file);
        } catch (const json::parse_error& e) {
            SPDLOG_ERROR("Failed to parse settings: {}", e.what());
            return false;
        }
    }

    return true;
}

fs::path Settings::getPath() {
    return path;
}

bool Settings::addUpdateDest(Destination dest) {
    // Check if destinationFolder exists
    if (!fs::exists(dest.destinationFolder)) {
        SPDLOG_ERROR("New destination folder doesn't exist");
        return false;
    }

    // Check if 'destinations' key exists in the json, if not create it.
    if (data.find("destinations") == data.end()) {
        data["destinations"] = nlohmann::json::array();
    }

    // Loop through the existing destinations to see if one with the same name already exists.
    bool exists = false;
    for (auto& destination : data["destinations"]) {
        if (destination["name"] == dest.name) {
            destination["destinationFolder"] = dest.destinationFolder.string();
            exists = true;
            break;
        }
    }

    // If it doesn't exist, append the new destination.
    if (!exists) {
        nlohmann::json newDest;
        newDest["name"] = dest.name;
        newDest["destinationFolder"] = dest.destinationFolder.string();
        data["destinations"].push_back(newDest);
    }

    std::ofstream outFile(path);
    if (!outFile.is_open()){
        SPDLOG_ERROR("Couldn't open settings file");
    } else {
        outFile << data.dump(4);
    }

    return true;
}

bool Settings::removeDest(Destination dest) {
    if (data.find("destinations") == data.end()) {
        SPDLOG_ERROR("No destinations to remove from");
        return false;
    }

    for (auto& destination : data["destinations"]) {
        if (destination["name"] == dest.name) {
            data["destinations"].erase(destination);

            std::ofstream outFile(path);
            if (!outFile.is_open()){
                SPDLOG_ERROR("Couldn't open settings file");
            } else {
                outFile << data.dump(4);
            }

            return true;
        }
    }

    SPDLOG_ERROR("Destination not found");
    return false;
}

std::vector<Destination> Settings::getDestVec() {
    std::vector<Destination> destinations;
    if (data.find("destinations") == data.end()) {
        SPDLOG_WARN("No destinations found");
        return destinations;
    }

    for (const auto& existingDest : data["destinations"]) {
        Destination dest;
        dest.name = existingDest["name"];
        dest.destinationFolder = existingDest["destinationFolder"].get<std::string>();
        destinations.push_back(dest);
    }

    return destinations;
}

Destination Settings::getDest(const std::string& name) {
    Destination dest;
    if (data.find("destinations") == data.end()) {
        SPDLOG_WARN("No destinations found");
        return dest;
    }

    for (auto& destination : data["destinations"]) {
        if (destination["name"] == name){
            dest.name = destination["name"];
            dest.destinationFolder = destination["destinationFolder"].get<std::string>();
            return dest;
        }

    }
    SPDLOG_ERROR("Couldn't find destination with name: {}", name);
    return dest;
}

// TODO: push recurrence and types into settings
bool Settings::addUpdateBackup(Backup backup) {
    // Check if 'backups' key exists in the json, if not create it.
    if (data.find("backups") == data.end()) {
        data["backups"] = nlohmann::json::array();
    }
    std::vector<std::string> destinationsStr;
    for (auto& destinationName : backup.getDestinations()){
        destinationsStr.push_back(destinationName);
    }
    std::vector<std::string> sourcesStr;
    for (auto& source : backup.getSources()){
        sourcesStr.push_back(source.string());
    }
    std::vector<nlohmann::json> schedsJson;
    for (auto& schedule : backup.getSchedules()) {
        schedsJson.push_back(schedule->toJson());
    }


    // Loop through the existing destinations to see if one with the same name already exists.
    bool exists = false;
    for (auto& existingBackup : data["backups"]) {
        if (existingBackup["name"] == backup.getName()) {
            existingBackup["destinations"] = destinationsStr;
            existingBackup["sources"] = sourcesStr;
            existingBackup["schedules"] = schedsJson;
            exists = true;
            break;
        }
    }

    // If it doesn't exist, append the new destination.
    if (!exists) {
        nlohmann::json newBackup;
        newBackup["name"] = backup.getName();
        newBackup["destinations"] = destinationsStr;
        newBackup["sources"] = sourcesStr;
        newBackup["schedules"] = schedsJson;
        data["backups"].push_back(newBackup);
    }

    std::ofstream outFile(path);
    if (!outFile.is_open()){
        SPDLOG_ERROR("Couldn't open settings file");
    } else {
        outFile << data.dump(4);
    }

    return true;
}

std::vector<Backup> Settings::getBackupVec() {
    std::vector<Backup> backups;
    if (data.find("backups") == data.end()) {
        SPDLOG_WARN("No backups found");
        return backups;
    }

    for (const auto& backup : data["backups"]) {

        // TODO: Read recurrence and types from settings
        BackupBuilder builder;
//        SPDLOG_INFO("Clown emoji");
        std::vector<std::string> dests = getBackupDests(backup["name"]);
        std::vector<fs::path> sources = getBackupSrcs(backup["name"]);
        std::vector<std::shared_ptr<BackupSchedule>> scheds = getBackupScheds(backup["name"]);

        auto backupObj = builder
                        .setName(backup["name"])
                        .setDestinations(dests)
                        .setSources(sources)
                        .setSchedules(scheds)
                        .buildSchedule();


        if(!backupObj) {
            SPDLOG_ERROR("Couldn't get a backup from settings");
            return backups;
        } else {
            backups.push_back(*backupObj);
        }

    }

    return backups;
}

std::vector<std::shared_ptr<BackupSchedule>> Settings::getBackupScheds(const std::string& name) {
    std::vector<std::shared_ptr<BackupSchedule>> scheds;
    if (data.find("backups") == data.end()) {
        SPDLOG_WARN("No backups found");
        return scheds;
    }

    for (auto& backup : data["backups"]) {
        if (backup["name"] == name) {
            for (auto& schedule : backup["schedules"]) {
                std::shared_ptr<BackupSchedule> scheduleObj;

                if (schedule["recurrence"] == "ONCE") {
                    auto obj = std::make_shared<OnceBackupSchedule>();
                    obj->type = schedule["type"];
                    obj->year = schedule["year"];
                    obj->month = schedule["month"];
                    obj->day = schedule["day"];
                    obj->hour = schedule["hour"];
                    obj->minute = schedule["minute"];
                    scheduleObj = obj;
                } else if (schedule["recurrence"] == "MONTHLY") {
                    auto obj = std::make_shared<MonthlyBackupSchedule>();
                    obj->type = schedule["type"];
                    obj->day = schedule["day"];
                    obj->hour = schedule["hour"];
                    obj->minute = schedule["minute"];
                    scheduleObj = obj;
                } else if (schedule["recurrence"] == "WEEKLY") {
                    auto obj = std::make_shared<WeeklyBackupSchedule>();
                    obj->type = schedule["type"];
                    obj->day = schedule["day"];
                    obj->hour = schedule["hour"];
                    obj->minute = schedule["minute"];
                    scheduleObj = obj;
                } else if (schedule["recurrence"] == "DAILY") {
                    auto obj = std::make_shared<DailyBackupSchedule>();
                    obj->type = schedule["type"];
                    obj->hour = schedule["hour"];
                    obj->minute = schedule["minute"];
                    scheduleObj = obj;
                }

                if (scheduleObj) {
                    scheds.push_back(scheduleObj);
                }
            }
            return scheds;
        }
    }

    SPDLOG_ERROR("Backup not found");

    return scheds;
}

std::vector<fs::path> Settings::getBackupSrcs(const std::string& name) {
    std::vector<fs::path> backupSrcs;
    if (data.find("backups") == data.end()) {
        SPDLOG_ERROR("No backups found");
        return backupSrcs;
    }

    for (auto& backup : data["backups"]) {
        if (backup["name"] == name) {
            for (auto& source : backup["sources"]) {
                fs::path srcPath = source.get<std::string>();
                backupSrcs.push_back(srcPath);
            }

            return backupSrcs;
        }
    }

    SPDLOG_ERROR("Backup not found");
    return backupSrcs;
}

std::vector<std::string> Settings::getBackupDests(const std::string& name) {
    std::vector<std::string> backupDests;
    if (data.find("backups") == data.end()) {
        SPDLOG_ERROR("No backups found");
        return backupDests;
    }

    for (auto& backup : data["backups"]) {
        if (backup["name"] == name) {
            for (auto& destination : backup["destinations"]) {
//                Destination dest;
//                dest.name = destination;
//                dest.destinationFolder = destination["destinationFolder"].get<std::string>();
                backupDests.push_back(destination);
            }

            return backupDests;
        }
    }

    SPDLOG_ERROR("Backup not found");
    return backupDests;
}

bool Settings::removeBackup(Backup backup) {
    if (data.find("backups") == data.end()) {
        SPDLOG_WARN("No backups found");
        return false;
    }

    for (auto& backupJson : data["backups"]) {
        if (backupJson["name"] == backup.getName()) {
            data["backups"].erase(backupJson);

            std::ofstream outFile(path);
            if (!outFile.is_open()){
                SPDLOG_ERROR("Couldn't open settings file");
            } else {
                outFile << data.dump(4);
            }

            return true;
        }
    }

    SPDLOG_ERROR("Backup not found");
    return false;
}
