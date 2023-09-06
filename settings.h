#ifndef SETTINGS_H
#define SETTINGS_H

#include <fstream>
#include <mutex>
#include <filesystem>
#include <optional>

#include "nlohmann/json.hpp"
#include "destination.h"
#include "task.h"

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

    bool addUpdate(Destination dest);
    bool remove(Destination dest);
    std::vector<Destination> getDestVec() const;
    Destination getDest(const std::string& key) const;

    bool addUpdate(Task task);
    bool remove(Task task);
    std::vector<Task> getTaskVec() const;

    std::vector<std::string> getKeyDests(const std::string& key) const;
    std::vector<fs::path> getKeySrcs(const std::string& key) const;
    std::vector<std::shared_ptr<Schedule>> getKeyScheds(const std::string& key) const;

    fs::path getPath() const;
private:
    json data = {
        {"destinations", json::array()},
        {"tasks", json::array()}
    };
    fs::path path;

};


#endif // SETTINGS_H
