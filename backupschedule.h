#ifndef BACKUPSCHEDULE_H
#define BACKUPSCHEDULE_H

#include <filesystem>
#include <windows.h>

#include "utils.h"

/**
 * @enum ScheduleRecurrence
 * @brief Frequency of scheduled backups.
 */
enum class ScheduleRecurrence {
    DAILY,   ///< Backup occurs daily at a specified hour and minute.
    WEEKLY,  ///< Backup occurs weekly on a specified weekday(1-7), hour, and minute.
    MONTHLY, ///< Backup occurs monthly on a specified day of the month, hour, and minute.
    ONCE,    ///< Backup is scheduled for a single occurrence on a specified date and time.
};

/**
 * @class BackupSchedule
 * @brief Represents a scheduled backup.
 *
 * This class defines a backup schedule with its type, directory, recurrence, and schedule data.
 * It provides methods to retrieve the backup details and generate task names and times.
 */
class BackupSchedule {
    friend class BackupFactory;

private:
    // Constructor made private to enforce factory pattern usage
    BackupSchedule(BackupType type, std::filesystem::path dir, ScheduleRecurrence recurrence, std::array<int, 5> scheduleData);

    BackupType type;                        ///< Type of the backup (e.g., FULL, INCREMENTAL).
    std::filesystem::path directory;        ///< Directory where the backup is stored.
    ScheduleRecurrence recurrence;          ///< Frequency of the backup.
    std::array<int, 5> scheduleData;        ///< Schedule data depending on recurrence (e.g., year, month, day, hour, minute).

public:

    /**
     * @brief Generates a task name of the backup in format needed for task scheduler.
     * @return A formatted string representing the backup task name without '\', '/', ':'.
     */
    std::wstring getTaskName();

    HRESULT addToTaskScheduler();
    bool deleteTask();

    // Getter methods for backup attributes
    std::filesystem::path getDirectory() const;
    BackupType getType() const;
    const std::string getTypeStr();
    ScheduleRecurrence getRecurrence() const;
    int getYear() const;
    int getMonth() const;
    int getDay() const;
    int getHour() const;
    int getMinute() const;

private:


    /**
     * @brief Generates the time for the backup task in format needed for task scheduler.
     * @return A wide string representing the backup task time in format "YYYY-MM-DDTHH:mm".
     */
    std::wstring getTaskTime();

};

#endif // BACKUPSCHEDULE_H
