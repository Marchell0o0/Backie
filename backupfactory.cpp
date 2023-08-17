#include "backupfactory.h"

#include <string>
#include <map>

// Define the static member
BackupFactory::ErrorCode BackupFactory::lastError = BackupFactory::ErrorCode::SUCCESS;

BackupFactory::ErrorCode BackupFactory::GetLastCreationError() {
    return lastError;
}

std::string BackupFactory::ErrorCodeToString(BackupFactory::ErrorCode errorCode) {
    static const std::map<BackupFactory::ErrorCode, std::string> errorCodeMap = {
        {BackupFactory::ErrorCode::SUCCESS, "SUCCESS"},
        {BackupFactory::ErrorCode::DIRECTORY_NOT_FOUND, "DIRECTORY_NOT_FOUND"},
        {BackupFactory::ErrorCode::INVALID_DATE_ARGUMENTS, "INVALID_DATE_ARGUMENTS"},
        {BackupFactory::ErrorCode::BAD_BACKUP_TYPE, "BAD_BACKUP_TYPE"}
        // ... other error codes ...
    };

    auto it = errorCodeMap.find(errorCode);
    if (it != errorCodeMap.end()) {
        return it->second;
    } else {
        return "UNKNOWN_ERROR";
    }
}
