#ifndef BACKUPBUILDER_H
#define BACKUPBUILDER_H

#include <optional>
#include <filesystem>

#include "utils.h"
#include "errand.h"
#include "task.h"
#include "destination.h"
#include "schedule.h"

class BackupBuilder
{
    friend class Settings;
public:
    BackupBuilder& setName(const std::string& name);
    BackupBuilder& setCurrentType(const BackupType currentType);
    BackupBuilder& setDestinations(const std::vector<Destination> destinations);
    BackupBuilder& setDestinations(const std::vector<std::string> destinationsKeys);
    BackupBuilder& setSources(const std::vector<fs::path> sources);
    BackupBuilder& setSchedules(std::vector<std::shared_ptr<Schedule>> schedules);
    BackupBuilder& setKey(const std::string& key);

    std::optional<Errand> buildErrand();

    std::optional<Task> buildTask();
private:
    std::string key = "";
    bool noNewKey = false;
    std::string name = "";
    BackupType currentType = BackupType::NONE;
    std::vector<std::string> destinationsKeys;
    std::vector<fs::path> sources;
    std::vector<std::shared_ptr<Schedule>> schedules;

    BackupBuilder& setNoNewKey();
    void reset();
};

#endif // BACKUPBUILDER_H
