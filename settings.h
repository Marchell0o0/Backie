#ifndef SETTINGS_H
#define SETTINGS_H

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <fstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class Settings {
public:
    json data;
    std::string path_to_settings = "W:/Programming/Backie/Backie/settings.json";

    /**
     * @brief Reads the settings data from the file.
     */
    void read_from_file();

    /**
     * @brief Writes the current settings data back to the file.
     */
    void push_changes();

    /**
     * @brief Adds or updates a backup task.
     * @param directory The directory to be backed up.
     * @param type The type of backup "scheduled"/"watched".
     * @param time The time to perform the backup, format: xx:xx (needed when scheduled).
     * @param filter Specific filter criteria for the backup (optional).
     */
    void backup_task(const std::string& directory, const std::string& type,
                     const std::string& time = "", const std::string& filter="");

    /**
     * @brief Removes a backup task by its directory.
     * @param directory The directory of the backup task to be removed.
     */
    void remove_backup_task(const std::string& directory);

    /**
     * @brief Sets the destination path for backups.
     * @param path_to_destination The destination path for backups.
     */
    void set_destination(const std::string& path_to_destination);

private:
    /**
     * @brief Updates a backup task with new or modified settings.
     * @param directory The directory to be backed up.
     * @param type The type of backup "scheduled"/"watched".
     * @param time The time to perform the backup, format: xx:xx (needed when scheduled).
     * @param filter Specific filter criteria for the backup (optional).
     */
    void update_backup_task(const std::string& directory, const std::string& type,
                            const std::string& time = "", const std::string& filter="");
};

#endif // SETTINGS_H
