#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <QApplication>
#include <QPushButton>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

#include "spdlog/spdlog.h"

#include "mainwindow.h"

#include "backup.h"
#include "backupschedule.h"
#include "backupfactory.h"

#include "utils.h"

//#include "settings.h"


int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::trace);

    if (argc > 1 && strcmp(argv[1], "--backup") == 0) {
        QCoreApplication app(argc, argv);

        if (argc != 4) {
            SPDLOG_ERROR("Wrong number of arguments");
            return 1;
        }

        std::filesystem::path directory = argv[2];

        BackupType type = Backup::getTypeFromStr(argv[3]);

        auto backup = BackupFactory::CreateBackup(type, directory);
        if (!backup) {
            SPDLOG_ERROR("Couldn't create backup. Error: {}",
                        BackupFactory::ErrorCodeToString(BackupFactory::GetLastCreationError()));
            return 1;
        }

        SPDLOG_INFO("Backup requested with type {}, directory {}", static_cast<int>(type), directory.u8string());

        // send message that backup has started

        //get in other arguments and call executeBackup function...

        return 0;
    } else {
        QApplication app(argc, argv);
        MainWindow w;

        auto backupShedule_test = BackupFactory::CreateBackupSchedule<ScheduleRecurrence::DAILY>(BackupType::FULL, "W:/backup_testing/source", 15, 25);
        if (backupShedule_test && backupShedule_test->addToTaskScheduler()){

            SPDLOG_INFO("Added backup_test");
        } else {
            SPDLOG_ERROR("Couldn't add backupShedule_test. Error: {}",
                        BackupFactory::ErrorCodeToString(BackupFactory::GetLastCreationError()));
        }

        SPDLOG_INFO("Drawing gui...");

        // get messages from the worker backup

        w.show();
        return app.exec();
    }
}


//    if (IsRunningAsAdmin()) {
//        SPDLOG_INFO("Running program as admin");
//    } else {
//        SPDLOG_INFO("Running program as user");
//    }

    /* Examples of settings.h */
    /*
    Settings settings;
    settings.read_from_file();
    settings.backup_task("W:/backup_testing/1", "scheduled", "18:00");
    settings.set_destination("W:/backup_testing/destination");
    settings.push_changes();
    */

    // TODO
    /* Duplicate task problems
     *
     * Simultaneous backup of the same folder. Lock file.
     *
     */



