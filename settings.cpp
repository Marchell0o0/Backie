#include "settings.h"
#include "spdlog/spdlog.h"

void Settings::read_from_file(){
    std::ifstream file(path_to_settings);
    if (file.is_open()) {
        data = json::parse(file);
    } else {
        SPDLOG_WARN("Creating new settings file");
        std::ofstream fout(path_to_settings);
        if (fout.is_open()) {
            fout << data.dump(4);
        } else {
            SPDLOG_ERROR("Could not create the file");
        }
    }
}

void Settings::push_changes(){
    std::ofstream fout(path_to_settings);
    if (fout.is_open()) {
        fout << data.dump(4);
    } else {
        SPDLOG_ERROR("Could not open the file for writing");
    }
}

void Settings::backup_task(const std::string& directory, const std::string& type, const std::string& time, const std::string& filter){
    // Check if a task with the same directory already exists
    for (const json& task : data["tasks"]) {
        if (task["directory"] == directory) {
            SPDLOG_DEBUG("Task with directory {} already exists", directory);
            update_backup_task(directory, type, time, filter);
            return;
        }
    }

    if(type == "scheduled" && time.length() != 5){
        SPDLOG_WARN("Wrong time {}, directory {} won't be added", time, directory);
        return;
    } else if ( type == "watched" && time != ""){
        SPDLOG_WARN("Time is set but task type is watched, directory {}", directory);
    }

    json task;
    task["directory"] = directory;
    task["type"] = type; // "scheduled" or "watched"
    task["filter"] = filter; // File extension or name filter

    if (type == "watched") {
        task["time"] = "";
    } else {
        task["time"] = time;
    }

    data["tasks"].push_back(task); // Add the task to the "tasks" array
    SPDLOG_DEBUG("Added a task for {}", directory);
}

// Update an existing backup task by directory
void Settings::update_backup_task(const std::string& directory, const std::string& type,
                        const std::string& time, const std::string& filter) {

    if(type == "scheduled" && time.length() != 5){
        SPDLOG_WARN("Wrong time {}, directory {} time won't be changed", time, directory);
    } else if ( type == "watched" && time != ""){
        SPDLOG_WARN("Time is set but task type is watched, directory {}");
    }

    for (json& task : data["tasks"]) {
        if (task["directory"] == directory) {
            task["type"] = type;
            task["filter"] = filter;

            if (type == "scheduled" && time.length() == 5) {
                task["time"] = time;
            } else {
                task["time"] = "";
            }

            SPDLOG_DEBUG("Updated a task for {}", directory);
            return;
        }
    }

    SPDLOG_ERROR("Task with directory {} not found", directory);
}

void Settings::remove_backup_task(const std::string& directory) {
    for (auto it = data["tasks"].begin(); it != data["tasks"].end(); ++it) {
        if ((*it)["directory"] == directory) {
            data["tasks"].erase(it);
            SPDLOG_DEBUG("Removed a task for {}", directory);
            return;
        }
    }

    SPDLOG_ERROR("Task with directory {} not found", directory);

}

void Settings::set_destination(const std::string& path_to_destination){
    data["destination"] = path_to_destination;
}
