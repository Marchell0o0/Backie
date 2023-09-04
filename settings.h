#ifndef SETTINGS_H
#define SETTINGS_H

#include <fstream>
#include <mutex>
#include <filesystem>
#include <optional>

#include "nlohmann/json.hpp"

#include "destination.h"
#include "backuperrand.h"
using json = nlohmann::json;


class Settings {
private:
    Settings() {}
public:
    Settings(Settings &other) = delete;
    void operator=(const Settings &) = delete;

    static Settings& getInstance() {
        static Settings instance;
        return instance;
    }

    bool initializeSettings();

    bool addUpdateDest(Destination dest);
    bool removeDest(Destination dest);
    std::vector<Destination> getDestVec();
    Destination getDest(const std::string& name);

    bool addUpdateBackup(Backup backup);
    bool removeBackup(Backup backup);

    std::vector<Backup> getBackupVec();

    std::vector<std::string> getBackupDests(const std::string& name);
    std::vector<fs::path> getBackupSrcs(const std::string& name);

    std::filesystem::path getPath();
private:
    json data = {
        {"destinations", json::array()},
        {"backups", json::array()}
    };
    std::filesystem::path path;


    std::vector<std::shared_ptr<BackupSchedule>> getBackupScheds(const std::string& name);
};


#endif // SETTINGS_H
