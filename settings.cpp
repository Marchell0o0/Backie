#include <windows.h>
#include <ShlObj.h>
#include <vector>

#include "settings.h"
#include "spdlog/spdlog.h"

void Settings::initialize_settings_path() {
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        // Form the path to the Backie folder in AppData
        std::string backieFolderPath = std::string(path) + "\\Backie";
        // Check if the directory exists and if not, create it
        if (GetFileAttributesA(backieFolderPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
            CreateDirectoryA(backieFolderPath.c_str(), NULL);
        }
        path_to_settings = backieFolderPath + "\\settings.json";
    } else {
        // Default to relative path if we can't get appdata
        path_to_settings = "./settings.json";

    }
    SPDLOG_INFO("Path to settings {}", path_to_settings);
    std::ifstream file(path_to_settings);
    if(!file.is_open()){
        SPDLOG_WARN("Creating new settings file");
        std::ofstream fout(path_to_settings);
        if (fout.is_open()) {
            fout << data.dump(4);
        } else {
            SPDLOG_ERROR("Could not create the file");
        }
    }

}

void Settings::read_from_file(){
    std::lock_guard<std::mutex> lock(file_mutex);
    std::ifstream file(path_to_settings);
    if (file.is_open()) {
        data = json::parse(file);
    } else {
        SPDLOG_ERROR("No settings file to read from");
    }
}

void Settings::push_changes(){
    std::lock_guard<std::mutex> lock(file_mutex);
    std::ofstream fout(path_to_settings);
    if (fout.is_open()) {
        fout << data.dump(4);
    } else {
        SPDLOG_ERROR("Could not open the file for writing");
    }
}

std::vector<Task> Settings::get_task_list_by_type(const std::string& type){
    std::vector<Task> tasks;

    if (type != "scheduled" && type != "watched"){
        return tasks;
    }

    for (const json& task_json : data["tasks"]){
        if (task_json["type"] == type){

            tasks.push_back(task_json_to_struct(task_json));
        }
    }
    return tasks;
}

Task Settings::task_json_to_struct(json task_json){
    Task task;
    task.directory = task_json["directory"];
    task.type = task_json["type"];
    task.time = task_json["time"];
    task.filter = task_json["filter"];
    return task;
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
