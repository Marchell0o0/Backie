/**
 * @file backup.h
 * @brief Backup Class
 *
 * This header file contains the definitions for managing and executing backups.
 */

#ifndef BACKUP_H
#define BACKUP_H

#include <filesystem>

#include "utils.h"

//#include <optional>
//#include <array>
//#include <stdexcept>
//#include <string>
//#include <iostream>

class Backup {
    friend class BackupFactory;

private:
    Backup(BackupType type, std::filesystem::path dir);

public:

    //TODO executeBackup()

    static BackupType getTypeFromStr(const std::string& string);

    std::filesystem::path getDirectory() const;
    BackupType getType() const;
private:
    BackupType type;
    std::filesystem::path directory;
};

#endif // BACKUP_H
