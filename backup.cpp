#include "backup.h"

#include <string>
#include <map>

Backup::Backup(BackupType type, const std::string& dir, ScheduleRecurrence recurrence, std::array<int, 5> scheduleData)
    : type(type), directory(dir), recurrence(recurrence), scheduleData(scheduleData) {
}

BackupType Backup::getType() const {
    return type;
}

const std::string& Backup::getDirectory() const {
    return directory;
}

ScheduleRecurrence Backup::getRecurrence() const {
    return recurrence;
}

int Backup::getYear() const {
    if (recurrence != ScheduleRecurrence::ONCE) {
        // handle error
    }
    return scheduleData[0];
}

int Backup::getMonth() const {
    if (recurrence != ScheduleRecurrence::ONCE) {
        // handle error
    }
    return scheduleData[1];
}

int Backup::getDay() const {
    switch (recurrence) {
    case ScheduleRecurrence::ONCE:
        return scheduleData[2];
    case ScheduleRecurrence::MONTHLY:
        return scheduleData[0];
    default:
        // handle error
        return 1;
    }
}

int Backup::getHour() const {
    switch (recurrence) {
    case ScheduleRecurrence::ONCE:
        return scheduleData[3];
    case ScheduleRecurrence::MONTHLY:
    case ScheduleRecurrence::WEEKLY:
        return scheduleData[1];
    case ScheduleRecurrence::DAILY:
        return scheduleData[0];
    default:
        // handle error
        return 1;
    }
}

int Backup::getMinute() const {
    switch (recurrence) {
    case ScheduleRecurrence::ONCE:
        return scheduleData[4];
    case ScheduleRecurrence::MONTHLY:
    case ScheduleRecurrence::WEEKLY:
        return scheduleData[2];
    case ScheduleRecurrence::DAILY:
        return scheduleData[1];
    default:
        // handle error
        return 1;
    }
}

std::string Backup::getTaskName() {
    std::string taskName;

    std::string recurrenceString;
    switch (recurrence) {
    case ScheduleRecurrence::ONCE:
        recurrenceString = "singular";
        break;
    case ScheduleRecurrence::MONTHLY:
        recurrenceString = "monthly";
        break;
    case ScheduleRecurrence::WEEKLY:
        recurrenceString = "weekly";
        break;
    case ScheduleRecurrence::DAILY:
        recurrenceString = "daily";
        break;
    }

    std::string typeString;
    switch (type) {
    case BackupType::FULL:
        typeString = "Full";
        break;
    case BackupType::INCREMENTAL:
        typeString = "Incremental";
        break;
    }

    taskName = typeString + " " + recurrenceString + " backup of " + directory;

    // Cleanup for task scheduler rules
    std::replace(taskName.begin(), taskName.end(), '/', ' ');
    std::replace(taskName.begin(), taskName.end(), '\\', ' ');
    taskName.erase(std::remove(taskName.begin(), taskName.end(), ':'), taskName.end());

    return taskName;
}

std::wstring Backup::getTaskTime() {
    std::wstring startTime;
    if (recurrence == ScheduleRecurrence::ONCE) {
        startTime = std::to_wstring(getYear()) + L"-" + std::to_wstring(getMonth()) + L"-" + std::to_wstring(getDay()) + L"T" +
                    std::to_wstring(getHour()) + L":" + std::to_wstring(getMinute()) + L":00";
    } else {
        startTime = L"2023-08-01T" + std::to_wstring(getHour()) + L":" + std::to_wstring(getMinute()) + L":00";
    }
    return startTime;
}

// Define the static member
BackupFactory::ErrorCode BackupFactory::lastError = BackupFactory::ErrorCode::SUCCESS;

BackupFactory::ErrorCode BackupFactory::GetLastError() {
    return lastError;
}


std::string BackupFactory::ErrorCodeToString(BackupFactory::ErrorCode errorCode) {
    static const std::map<BackupFactory::ErrorCode, std::string> errorCodeMap = {
        {BackupFactory::ErrorCode::SUCCESS, "SUCCESS"},
        {BackupFactory::ErrorCode::DIRECTORY_NOT_FOUND, "DIRECTORY_NOT_FOUND"},
        {BackupFactory::ErrorCode::INVALID_DATE_ARGUMENTS, "INVALID_DATE_ARGUMENTS"},
        // ... other error codes ...
    };

    auto it = errorCodeMap.find(errorCode);
    if (it != errorCodeMap.end()) {
        return it->second;
    } else {
        return "UNKNOWN_ERROR";
    }
}
