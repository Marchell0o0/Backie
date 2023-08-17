#include "backup.h"

#include <string>

Backup::Backup(BackupType type, std::filesystem::path dir)
    : type(type), directory(dir) {
}


std::filesystem::path Backup::getDirectory() const  {
    return directory;
}

BackupType Backup::getType() const {
    return type;
}

BackupType Backup::getTypeFromStr(const std::string& string) {
    BackupType type = BackupType::NONE;
    if (string == "FULL"){
        type = BackupType::FULL;
    } else if (string == "INCREMENTAL") {
        type = BackupType::INCREMENTAL;
        // other types
    }
    return type;
}
