#include <windows.h>
#include <ShlObj.h>
#include <vector>

#include "spdlog/spdlog.h"

#include "backupbuilder.h"
#include "settings.h"
#include "utils.h"

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

fs::path Settings::getPath() const {
    return path;
}

bool Settings::addUpdate(Destination dest) {
    // Check if destinationFolder exists
    if (!fs::exists(dest.destinationFolder)) {
        SPDLOG_ERROR("New destination folder doesn't exist");
        return false;
    }

    if (data.find("destinations") == data.end()) {
        SPDLOG_WARN("No destinations found");
        return false;
    }

    // Loop through the existing destinations to see if one with the same name already exists.
    bool same_key = false;
    bool same_name = false;
    bool same_folder = false;

    for (auto& destination : data["destinations"]) {
        if (destination["key"] == dest.getKey()) {
            same_key = true;
        }
        if (destination["name"] == dest.name) {
            //exists something with different key but the same name
            same_name = true;
        }
        if (destination["destinationFolder"] == dest.destinationFolder.string()) {
            same_folder = true;
        }
    }
    if ((same_folder || same_name) && !same_key) {
        //exists something with the same name or folder
        SPDLOG_ERROR("Name or folder already taken");
        return false;
    } else if (same_key){
        // change existing destination
        for (auto& destination : data["destinations"]) {
            destination["name"] = dest.name;
            destination["destinationFolder"] = dest.destinationFolder.string();
        }
    } else if (!same_folder && !same_name && !same_key) {
        // everything is new
        nlohmann::json newDest;
        newDest["key"] = dest.getKey();
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

bool Settings::remove(Destination dest) {
    if (data.find("destinations") == data.end()) {
        SPDLOG_ERROR("No destinations to remove from");
        return false;
    }

    size_t index_to_remove = -1;
    size_t i = 0;
    for (const auto& destJson : data["destinations"]) {
        if (destJson["key"] == dest.getKey()) {
            index_to_remove = i;
            break;
        }
        ++i;
    }

    if (index_to_remove != -1) {
        data["destinations"].erase(index_to_remove);

        std::ofstream outFile(path);
        if (!outFile.is_open()) {
            SPDLOG_ERROR("Couldn't open settings file");
            return false;
        } else {
            outFile << data.dump(4);
        }
        return true;
    } else {
        SPDLOG_ERROR("Destination not found");
        return false;
    }
}

std::vector<Destination> Settings::getDestVec() const {
    std::vector<Destination> destinations;
    if (data.find("destinations") == data.end()) {
        SPDLOG_WARN("No destinations found");
        return destinations;
    }

    for (const auto& destination : data["destinations"]) {
        std::string name = destination["name"];
        std::filesystem::path destFldr = destination["destinationFolder"].get<std::string>();
        std::string key = destination["key"];

        Destination dest(name, destFldr, key);
        destinations.push_back(dest);
    }

    return destinations;
}

//TODO: That's why that constructor was needed
Destination Settings::getDest(const std::string& key) const {
    Destination dest;
    if (data.find("destinations") == data.end()) {
        SPDLOG_WARN("No destinations found");
        return dest;
    }

    for (auto& destination : data["destinations"]) {
        if (destination["key"] == key){
            std::string name = destination["name"];
            std::filesystem::path destFldr = destination["destinationFolder"].get<std::string>();
            Destination truedest(name, destFldr);
            return truedest;
        }

    }
    SPDLOG_ERROR("Couldn't find destination with key: {}", key);
    return dest;
}

bool Settings::addUpdate(Task task) {
    if (data.find("tasks") == data.end()) {
        SPDLOG_WARN("No tasks found");
        return false;
    }

    std::vector<std::string> destinationsStr;
    for (auto& destinationName : task.getDestinations()){
        destinationsStr.push_back(destinationName);
    }
    std::vector<std::string> sourcesStr;
    for (auto& source : task.getSources()){
        sourcesStr.push_back(source.string());
    }
    std::vector<nlohmann::json> schedsJson;
    for (auto& schedule : task.getSchedules()) {
        schedsJson.push_back(schedule->toJson());
    }

    bool exists = false;
    for (auto& existingTask : data["tasks"]) {
        if (existingTask["key"] == task.getKey()) {
            existingTask["name"] = task.getName();
            existingTask["destinations"] = destinationsStr;
            existingTask["sources"] = sourcesStr;
            existingTask["schedules"] = schedsJson;
            exists = true;
            break;
        }
    }

    if (!exists) {
        nlohmann::json newTask;
        newTask["key"] = task.getKey();
        newTask["name"] = task.getName();
        newTask["destinations"] = destinationsStr;
        newTask["sources"] = sourcesStr;
        newTask["schedules"] = schedsJson;
        data["tasks"].push_back(newTask);
    }

    std::ofstream outFile(path);
    if (!outFile.is_open()){
        SPDLOG_ERROR("Couldn't open settings file");
    } else {
        outFile << data.dump(4);
    }

    return true;
}

std::vector<Task> Settings::getTaskVec() const {
    std::vector<Task> tasks;
    if (data.find("tasks") == data.end()) {
        SPDLOG_WARN("No tasks found");
        return tasks;
    }

    for (const auto& task : data["tasks"]) {
        BackupBuilder builder;
        std::vector<std::string> dests = getKeyDests(task["key"]);
        std::vector<fs::path> sources = getKeySrcs(task["key"]);
        std::vector<std::shared_ptr<Schedule>> scheds = getKeyScheds(task["key"]);

        auto taskObj = builder
                        .setNoNewKey()
                        .setKey(task["key"])
                        .setName(task["name"])
                        .setDestinations(dests)
                        .setSources(sources)
                        .setSchedules(scheds)
                        .buildTask();

        if(!taskObj) {
            SPDLOG_ERROR("Couldn't get a task from settings");
            return tasks;
        } else {
            tasks.push_back(*taskObj);
        }

    }

    return tasks;
}

std::vector<std::shared_ptr<Schedule>> Settings::getKeyScheds(const std::string& key) const {
    std::vector<std::shared_ptr<Schedule>> scheds;
    if (data.find("tasks") == data.end()) {
        SPDLOG_WARN("No tasks found");
        return scheds;
    }

    for (auto& task : data["tasks"]) {
        if (task["key"] == key) {
            for (auto& schedule : task["schedules"]) {
                std::shared_ptr<Schedule> scheduleObj;

                if (schedule["recurrence"] == ScheduleRecurrence::ONCE) {
                    std::shared_ptr<OnceSchedule> obj = std::make_shared<OnceSchedule>();
                    obj->type = schedule["type"];
                    obj->year = schedule["year"];
                    obj->month = schedule["month"];
                    obj->day = schedule["day"];
                    obj->hour = schedule["hour"];
                    obj->minute = schedule["minute"];
                    scheduleObj = obj;
                } else if (schedule["recurrence"] == ScheduleRecurrence::MONTHLY) {
                    std::shared_ptr<MonthlySchedule> obj = std::make_shared<MonthlySchedule>();
                    obj->type = schedule["type"];
                    obj->day = schedule["day"];
                    obj->hour = schedule["hour"];
                    obj->minute = schedule["minute"];
                    scheduleObj = obj;
                } else if (schedule["recurrence"] == ScheduleRecurrence::WEEKLY) {
                    std::shared_ptr<WeeklySchedule> obj = std::make_shared<WeeklySchedule>();
                    obj->type = schedule["type"];
                    obj->day = schedule["day"];
                    obj->hour = schedule["hour"];
                    obj->minute = schedule["minute"];
                    scheduleObj = obj;
                } else if (schedule["recurrence"] == ScheduleRecurrence::DAILY) {
                    std::shared_ptr<DailySchedule> obj = std::make_shared<DailySchedule>();
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

    SPDLOG_ERROR("Task not found");

    return scheds;
}

std::vector<fs::path> Settings::getKeySrcs(const std::string& key) const {
    std::vector<fs::path> srcs;
    if (data.find("tasks") == data.end()) {
        SPDLOG_ERROR("No tasks found");
        return srcs;
    }

    for (auto& task : data["tasks"]) {
        if (task["key"] == key) {
            for (auto& source : task["sources"]) {
                fs::path srcPath = source.get<std::string>();
                srcs.push_back(srcPath);
            }

            return srcs;
        }
    }

    SPDLOG_ERROR("Task not found");
    return srcs;
}

std::vector<std::string> Settings::getKeyDests(const std::string& key) const {
    std::vector<std::string> dests;
    if (data.find("tasks") == data.end()) {
        SPDLOG_ERROR("No tasks found");
        return dests;
    }

    for (auto& task : data["tasks"]) {
        if (task["key"] == key) {
            for (auto& destination : task["destinations"]) {
                dests.push_back(destination);
            }

            return dests;
        }
    }

    SPDLOG_ERROR("Task not found");
    return dests;
}

bool Settings::remove(Task task) {
    if (data.find("tasks") == data.end()) {
        SPDLOG_WARN("No tasks found");
        return false;
    }

    size_t index_to_remove = -1;
    size_t i = 0;
    for (const auto& taskJson : data["tasks"]) {
        if (taskJson["key"] == task.getKey()) {
            index_to_remove = i;
            break;
        }
        ++i;
    }

    if (index_to_remove != -1) {
        data["tasks"].erase(index_to_remove);

        std::ofstream outFile(path);
        if (!outFile.is_open()) {
            SPDLOG_ERROR("Couldn't open settings file");
            return false;
        } else {
            outFile << data.dump(4);
        }
        return true;
    } else {
        SPDLOG_ERROR("Task not found");
        return false;
    }
}
