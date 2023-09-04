#ifndef BACKUPBUILDER_H
#define BACKUPBUILDER_H

#include <optional>
#include <filesystem>

#include "spdlog/spdlog.h"
#include "utils.h"
#include "backuperrand.h"

class BackupBuilder
{
private:
    std::string name = "";
    BackupType currentType = BackupType::NONE;
    std::vector<std::string> destinations;
    std::vector<fs::path> sources;
    std::vector<std::shared_ptr<BackupSchedule>> schedules;

    void reset() {
        this->name = "";
        this->currentType = BackupType::NONE;
        this->destinations.clear();
        this->sources.clear();
        this->schedules.clear();
    }

public:
    BackupBuilder& setChanging(Backup backup) {
        this->name = backup.getName();
        this->currentType = backup.getCurrentType();
        this->destinations = backup.getDestinations();
        this->sources = backup.getSources();
        this->schedules = backup.getSchedules();

        return *this;
    }
    BackupBuilder& setName(const std::string& name) {
        this->name = name;
        return *this;
    }
    BackupBuilder& setCurrentType(const BackupType currentType) {
        this->currentType = currentType;
        return *this;
    }
    BackupBuilder& setDestinations(const std::vector<std::string> destinations) {
        this->destinations = destinations;
        return *this;
    }
    BackupBuilder& setSources(const std::vector<fs::path> sources) {
        this->sources = sources;
        return *this;
    }

    BackupBuilder& setSchedules(std::vector<std::shared_ptr<BackupSchedule>> schedules) {
        this->schedules = schedules;
        return *this;
    }

    std::optional<Backup> buildErrand() {
        if (this->destinations.empty()){
            SPDLOG_ERROR("No destinations specified");
            return std::nullopt;
        }
        if (this->sources.empty()){
            SPDLOG_ERROR("No sources specified");
            return std::nullopt;
        }

//        for (auto& destination : this->destinations) {
//            if(!fs::exists(destination.destinationFolder)
//                || !fs::is_directory(destination.destinationFolder)){
//                SPDLOG_ERROR("Destination folder doesn't exist");
//                return std::nullopt;
//            }
//        }
        for (auto& source : this->sources) {
            if(!fs::exists(source) || !fs::is_directory(source)){
                SPDLOG_ERROR("Source doesn't exist");
                return std::nullopt;
            }
        }

        if (this->currentType == BackupType::NONE) {
            SPDLOG_ERROR("No current type specified");
            return std::nullopt;
        }

        Backup backup;
        backup.name = this->name;
        backup.currentType = this->currentType;
        backup.sources = this->sources;
        backup.destinations = this->destinations;

        this->reset();

        return backup;
    }

    std::optional<Backup> buildSchedule() {
        //TODO: other name checks
        if (this->name == "") {
            SPDLOG_ERROR("The backup name can't be empty");
        }

        if (this->destinations.empty()){
            SPDLOG_ERROR("No destinations specified");
            return std::nullopt;
        }
        if (this->sources.empty()){
            SPDLOG_ERROR("No sources specified");
            return std::nullopt;
        }

//        for (auto& destination : this->destinations) {
//            if(!fs::exists(destination.destinationFolder)
//                || !fs::is_directory(destination.destinationFolder)){
//                SPDLOG_ERROR("Destination folder doesn't exist");
//                return std::nullopt;
//            }
//        }
        for (auto& source : this->sources) {
            if(!fs::exists(source) || !fs::is_directory(source)){
                SPDLOG_ERROR("Source doesn't exist");
                return std::nullopt;
            }
        }

//        if (!(hour >= 0 && hour < 24 && minute >= 0 && minute < 60)){
//            SPDLOG_ERROR("Wrong hour or minute");
//            return std::nullopt;
//        }

//        switch(this->recurrence){
//        case ScheduleRecurrence::DAILY:
//            break;
//        case ScheduleRecurrence::WEEKLY:
//            if (day < 1 || day > 7) {
//                SPDLOG_ERROR("Wrong day for weekly");
//                return std::nullopt;
//            }
//            break;
//        case ScheduleRecurrence::MONTHLY:
//            if (day != -1 && (day < 1 || day > 31)) {
//                SPDLOG_ERROR("Wrong day for monthly");
//                return std::nullopt;
//            }
//            break;
//        case ScheduleRecurrence::ONCE:
//            if (month > 12 || month < 1 || year > 9999 || year < 1900 || day < 1 || day > 31) {
//                SPDLOG_ERROR("Wrong date for once");
//                return std::nullopt;
//            }
//            break;
//        case ScheduleRecurrence::NONE:
//            SPDLOG_ERROR("No recurrence specified");
//            return std::nullopt;
//        }

        Backup backup;
        backup.name = this->name;
        backup.currentType = this->currentType;
        backup.destinations = this->destinations;
        backup.sources = this->sources;
        backup.schedules = this->schedules;

        this->reset();

        return backup;
    }
};

#endif // BACKUPBUILDER_H
