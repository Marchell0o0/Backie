#ifndef BACKUPERRAND_H
#define BACKUPERRAND_H

#include <filesystem>
#include <windows.h>

#include "nlohmann/json.hpp"

#include "utils.h"

namespace fs = std::filesystem;

enum class ScheduleRecurrence {
    DAILY,
    WEEKLY,
    MONTHLY,
    ONCE,
//    NONE
};

class BackupSchedule {
public:
    virtual ~BackupSchedule() {}
    virtual nlohmann::json toJson() const = 0;
    BackupType type;
};

class OnceBackupSchedule : public BackupSchedule {
public:
    int year;
    int month;
    int day;
    int hour;
    int minute;
    nlohmann::json toJson() const override {
        return nlohmann::json{
            {"recurrence", "ONCE"},
            {"type", type},
            {"year", year},
            {"month", month},
            {"day", day},
            {"hour", hour},
            {"minute", minute}
        };
    }
private:
    ScheduleRecurrence recurrence = ScheduleRecurrence::ONCE;
};

class MonthlyBackupSchedule : public BackupSchedule {
public:
    int day;
    int hour;
    int minute;
    nlohmann::json toJson() const override {
        return nlohmann::json{
            {"recurrence", "MONTHLY"},
            {"type", type},
            {"day", day},
            {"hour", hour},
            {"minute", minute}
        };
    }
private:
    ScheduleRecurrence recurrence = ScheduleRecurrence::MONTHLY;
};

class WeeklyBackupSchedule : public BackupSchedule {
public:
    int day;
    int hour;
    int minute;
    nlohmann::json toJson() const override {
        return nlohmann::json{
            {"recurrence", "WEEKLY"},
            {"type", type},
            {"day", day},
            {"hour", hour},
            {"minute", minute}
        };
    }
private:
    ScheduleRecurrence recurrence = ScheduleRecurrence::WEEKLY;
};

class DailyBackupSchedule : public BackupSchedule {
public:
    int hour;
    int minute;
    nlohmann::json toJson() const override {
        return nlohmann::json{
            {"recurrence", "DAILY"},
            {"type", type},
            {"hour", hour},
            {"minute", minute}
        };
    }
private:
    ScheduleRecurrence recurrence = ScheduleRecurrence::DAILY;
};



class Backup {
    friend class BackupBuilder;

public:
    bool performBackup();
    HRESULT addToTaskScheduler();

    std::string getName();
    BackupType getCurrentType() const;
    std::vector<std::string> getDestinations() const;
    std::vector<fs::path> getSources() const;
    std::vector<std::shared_ptr<BackupSchedule>> getSchedules() const;
private:
    Backup() {};

    std::string name;

    BackupType currentType = BackupType::NONE;
    std::vector<std::string> destinations;
    std::vector<fs::path> sources;


    std::vector<std::shared_ptr<BackupSchedule>> schedules;

    bool fullBackup();
    bool incrementalBackup();
    bool backup(const fs::path file, const fs::path backupFolder, const fs::path sourcePath);
    std::string formatBackupFolderName();



    bool deleteTask();
    std::wstring getTaskName();
    std::wstring getTaskTime();
};

#endif // BACKUPERRAND_H
