#include "pch.h"

// #include <windows.h>
#include <ShlObj.h>
// #include <vector>

// #include "spdlog/spdlog.h"

#include "backupbuilder.h"
#include "settings.h"

void Settings::saveToFile() const {
    std::ofstream outFile(path);
    if (!outFile.is_open()) {
        SPDLOG_ERROR("Couldn't open settings file");
    } else {
        outFile << data.dump(4);
    }
}

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
    if (!fs::exists(dest.destinationFolder)) {
        SPDLOG_ERROR("New destination folder doesn't exist");
        return false;
    }

    bool exists;
    for (const auto& [key, destination] : data["destinations"].items()) {
        // if it's an update, then having the same name or folder isn't an error
        if (key == dest.getKey()) {
            exists = true;
            continue;
        }
        if (destination["name"] == dest.name
            || destination["destinationFolder"] == dest.destinationFolder.string()) {
            SPDLOG_ERROR("Name or folder already taken");
            return false;
        }
    }

    if (exists) {
        auto existingDest = data["destinations"][dest.getKey()];
        existingDest["name"] = dest.name;
        existingDest["destinationFolder"] = dest.destinationFolder.string();
    } else {
        nlohmann::json newDest;
        newDest["name"] = dest.name;
        newDest["destinationFolder"] = dest.destinationFolder.string();
        data["destinations"][dest.getKey()] = newDest;
    }

    saveToFile();
    return true;
}

bool Settings::remove(Destination dest) {
    if (data["destinations"].find(dest.getKey()) == data["destinations"].end()) {
        SPDLOG_ERROR("Destination not found");
        return false;
    }

    data["destinations"].erase(dest.getKey());
    saveToFile();
    return true;
}

std::vector<Destination> Settings::getDestVec() const {
    std::vector<Destination> destinations;

    for (const auto& [key, destination] : data["destinations"].items()) {
        Destination dest(destination["name"],
                         destination["destinationFolder"].get<std::string>(),
                         key);
        destinations.push_back(dest);
    }

    return destinations;
}

std::optional<Destination> Settings::getDest(const std::string& key) const {
    if (data["destinations"].find(key) != data["destinations"].end()) {
        const auto& destination = data["destinations"][key];
        Destination dest(destination["name"],
                         destination["destinationFolder"].get<std::string>(),
                         key);
        return dest;
    }

    SPDLOG_ERROR("Couldn't find destination with key: {}", key);
    return std::nullopt;
}


bool Settings::addUpdate(Task task) {

    std::vector<std::string> destinationsStr = task.getDestinations();

    std::vector<std::string> sourcesStr;
    for (auto& source : task.getSources()){
        sourcesStr.push_back(source.string());
    }
    std::vector<nlohmann::json> schedsJson;
    for (auto& schedule : task.getSchedules()) {
        schedsJson.push_back(schedule->toJson());
    }

    bool exists = false;
    for (const auto& [key, taskJson] : data["tasks"].items()) {
        if (key == task.getKey()) {
            exists = true;
            continue;
        }
        if (taskJson["name"] == task.getName()) {
            SPDLOG_ERROR("Task name already taken");
            return false;
        }
    }

    if (exists) {
        auto existingTask = data["tasks"][task.getKey()];
//        existingTask["latestId"] = task.getLatestId();
        existingTask["name"] = task.getName();
        existingTask["destinations"] = destinationsStr;
        existingTask["sources"] = sourcesStr;
        existingTask["schedules"] = schedsJson;
    } else {
        nlohmann::json newTask;
        newTask["latestId"] = "";
        newTask["name"] = task.getName();
        newTask["destinations"] = destinationsStr;
        newTask["sources"] = sourcesStr;
        newTask["schedules"] = schedsJson;
        data["tasks"][task.getKey()] = newTask;
    }

    saveToFile();
    return true;
}

bool Settings::remove(Task task) {
    if (!taskKeyInSettings(task.getKey())) {
        SPDLOG_ERROR("Task not found");
        return false;
    }

    data["tasks"].erase(task.getKey());
    saveToFile();
    return true;
}

std::vector<Task> Settings::getTaskVec() const {
    std::vector<Task> tasks;
    for (const auto& [key, task] : data["tasks"].items()) {
        std::vector<std::string> dests = getKeyDests(key);
        std::vector<fs::path> sources = getKeySrcs(key);
        std::vector<std::shared_ptr<Schedule>> scheds = getKeyScheds(key);

        BackupBuilder builder;
        auto taskObj = builder
                        .setNoNewKey()
                        .setKey(key)
//                        .setLatestId(task["latestId"])
                        .setName(task["name"])
                        .setDestinations(dests)
                        .setSources(sources)
                        .setSchedules(scheds)
                        .buildTask();

        if(!taskObj) {
            SPDLOG_ERROR("Couldn't get a task from settings");
            //TODO: Delete unwanted task
        } else {
            tasks.push_back(*taskObj);
        }
    }

    return tasks;
}

std::string Settings::getKeyName(const std::string& key) const {
    return data["tasks"][key]["name"];
}

std::string Settings::getKeyLatestId(const std::string& key) const {
    return data["tasks"][key]["latestId"];
}

bool Settings::setLatestId(const std::string& key, const std::string& id) {
    if (!taskKeyInSettings(key)) {
        SPDLOG_ERROR("Task not found");
        return false;
    }
    data["tasks"][key]["latestId"] = id;
    saveToFile();
    return true;
}

bool Settings::taskKeyInSettings(const std::string& key) const {
    return data["tasks"].find(key) != data["tasks"].end();
}

std::vector<std::shared_ptr<Schedule>> Settings::getKeyScheds(const std::string& key) const {
    std::vector<std::shared_ptr<Schedule>> scheds;
    for (auto& schedule : data["tasks"][key]["schedules"]) {
        std::shared_ptr<Schedule> scheduleObj;

        if (schedule["recurrence"] == ScheduleRecurrence::ONCE) {
            std::shared_ptr<OnceSchedule> obj = std::make_shared<OnceSchedule>();
//            obj->type = schedule["type"];
            obj->year = schedule["year"];
            obj->month = schedule["month"];
            obj->day = schedule["day"];
            obj->hour = schedule["hour"];
            obj->minute = schedule["minute"];
            scheduleObj = obj;
        } else if (schedule["recurrence"] == ScheduleRecurrence::MONTHLY) {
            std::shared_ptr<MonthlySchedule> obj = std::make_shared<MonthlySchedule>();
//            obj->type = schedule["type"];
            obj->day = schedule["day"];
            obj->hour = schedule["hour"];
            obj->minute = schedule["minute"];
            scheduleObj = obj;
        } else if (schedule["recurrence"] == ScheduleRecurrence::WEEKLY) {
            std::shared_ptr<WeeklySchedule> obj = std::make_shared<WeeklySchedule>();
//            obj->type = schedule["type"];
            obj->day = schedule["day"];
            obj->hour = schedule["hour"];
            obj->minute = schedule["minute"];
            scheduleObj = obj;
        } else if (schedule["recurrence"] == ScheduleRecurrence::DAILY) {
            std::shared_ptr<DailySchedule> obj = std::make_shared<DailySchedule>();
//            obj->type = schedule["type"];
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

std::vector<fs::path> Settings::getKeySrcs(const std::string& key) const {
    std::vector<fs::path> srcs;
    for (auto& source : data["tasks"][key]["sources"]) {
        fs::path srcPath = source.get<std::string>();
        srcs.push_back(srcPath);
    }
    return srcs;
}

std::vector<std::string> Settings::getKeyDests(const std::string& key) const {
    std::vector<std::string> dests;
    for (auto& destination : data["tasks"][key]["destinations"]) {
        dests.push_back(destination);
    }
    return dests;
}

