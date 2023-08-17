#ifndef BACKUPFACTORY_H
#define BACKUPFACTORY_H

#include <optional>

#include "backupschedule.h"
#include "backup.h"

/**
 * @class BackupFactory
 * @brief Factory class to create and manage backup objects.
 *
 * This class provides a mechanism to create backup objects after validating the input parameters.
 * It enforces the factory pattern, ensuring backup objects are created with valid configurations.
 */
class BackupFactory {
public:
    enum class ErrorCode {
        SUCCESS,
        DIRECTORY_NOT_FOUND,
        INVALID_DATE_ARGUMENTS,
        BAD_BACKUP_TYPE,
        // ... other error codes ...
    };

    /**
     * @brief Creates a backup object after validation.
     *
     * Template function that validates input arguments based on backup recurrence and creates a backup object.
     * If there is an error in the input arguments, sets last error variable(obtainable by GetLastError()) to a certain ErrorCode.
     *
     * @tparam R Recurrence type of the backup.
     * @tparam Args Variadic template for schedule arguments (e.g., hour, minute).
     *
     * @param type Type of the backup.
     * @param directory Directory for the backup.
     * @param args Date arguments. Array of ints of changing size. In this order year, month, day, hour, minute
     *
     * Month could be equal to -1, which means the task should run on the last day of every month.
     *
     * @return An optional containing a Backup object if creation is successful, or std::nullopt if not.
     */
    template <ScheduleRecurrence R, typename... Args>
    static std::optional<BackupSchedule> CreateBackupSchedule(BackupType type, std::filesystem::path directory, Args... args){
        if (!std::filesystem::exists(directory)) {
            lastError = ErrorCode::DIRECTORY_NOT_FOUND;
            return std::nullopt;
        }

        if (!validateDateArgs<R>(args...)) {
            lastError = ErrorCode::INVALID_DATE_ARGUMENTS;
            return std::nullopt;
        }

        std::array<int, 5> data = {0, 0, 0, 0, 0}; // initialize with zeros
        int temp[] = {args...};
        for (size_t i = 0; i < sizeof...(args); ++i) {
            data[i] = temp[i];
        }
        lastError = ErrorCode::SUCCESS;
        return BackupSchedule(type, directory, R, data);
    }

    static std::optional<Backup> CreateBackup(BackupType type, std::filesystem::path directory){
        if (!std::filesystem::exists(directory)) {
            lastError = ErrorCode::DIRECTORY_NOT_FOUND;
            return std::nullopt;
        }
        if (type == BackupType::NONE) {
            lastError = ErrorCode::BAD_BACKUP_TYPE;
            return std::nullopt;
        }
        return Backup(type, directory);
    }

    /**
     * @brief Retrieves the last error code set during backup creation.
     * @return The error code representing the last error.
     */
    static ErrorCode GetLastCreationError();

    /**
     * @brief Make an enum class member into a string
     * @param Error code from ErrorCode enum
     * @return Error string
     */
    static std::string ErrorCodeToString(BackupFactory::ErrorCode errorCode);

private:
    static ErrorCode lastError;  ///< Static member to store the last error code.

    /**
     * @brief Validates date arguments based on backup recurrence.
     *
     * Template function that checks the validity of schedule arguments depending on the recurrence.
     *
     * @tparam R Recurrence type of the backup.
     * @tparam Args Variadic template for schedule arguments.
     *
     * @param args Schedule arguments depending on recurrence.
     *
     * @return True if arguments are valid, false otherwise.
     */
    template <ScheduleRecurrence R, typename... Args>
    static bool validateDateArgs(Args... args){
        int temp[] = {args...};  // Expand the pack into an array

        if constexpr (R == ScheduleRecurrence::DAILY) {
            if (sizeof...(Args) != 2) return false;
            int hour = temp[0], minute = temp[1];
            return hour >= 0 && hour < 24 && minute >= 0 && minute < 60;

        } else if constexpr (R == ScheduleRecurrence::WEEKLY) {
            if (sizeof...(Args) != 3) return false;
            int dayOfWeek = temp[0], hour = temp[1], minute = temp[2]; // e.g., 1 = Monday
            return dayOfWeek >= 1 && dayOfWeek <= 7 && hour >= 0 && hour < 24 && minute >= 0 && minute < 60;

        } else if constexpr (R == ScheduleRecurrence::MONTHLY) {
            if (sizeof...(Args) != 3) return false;
            int dayOfMonth = temp[0], hour = temp[1], minute = temp[2];
            return (dayOfMonth >= 1 || dayOfMonth == -1) && dayOfMonth <= 31 && hour >= 0 && hour < 24 && minute >= 0 && minute < 60;

        } else if constexpr (R == ScheduleRecurrence::ONCE) {
            if (sizeof...(Args) != 5) return false;
            int year = temp[0], month = temp[1], day = temp[2], hour = temp[3], minute = temp[4];
            // Basic date validation. More precise validation would involve checking how many days a particular month has.
            return day >= 1 && day <= 31 && month >= 1 && month <= 12 && hour >= 0 && hour < 24 && minute >= 0 && minute < 60;
        }
        return false;
    }

};



#endif // BACKUPFACTORY_H
