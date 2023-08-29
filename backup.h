/**
 * @file backup.h
 * @brief Backup Class
 *
 * This header file contains the definitions for managing and executing backups.
 */

#ifndef BACKUP_H
#define BACKUP_H

#include <filesystem>
#include <optional>

//#include "metadata.h"
#include "utils.h"

namespace fs = std::filesystem;

class Backup {
    friend class BackupFactory;

private:
    Backup(BackupType type, fs::path dir);

public:
    bool performBackup();

    fs::path getDirectory() const;
    BackupType getType() const;
private:
    BackupType type;
    fs::path globalDestination = "W:\\Backie backups\\Dest 1";
//    fs::path globalDestination = "D:\\Code\\Backie_backups\\Dest_1";
    fs::path directory;

    bool fullBackup();
    bool incrementalBackup();
    bool backup(const fs::path file, const fs::path backupFolder);
    std::string formatBackupFolderName();
};

#endif // BACKUP_H
