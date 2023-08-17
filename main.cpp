#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <QApplication>
#include <QPushButton>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <windows.h>

#include "spdlog/spdlog.h"

#include "mainwindow.h"
#include "backup.h"
//#include "settings.h"
#include "taskscheduler.h"


bool IsRunningAsAdmin() {
    BOOL isRunAsAdmin = FALSE;
    PSID adminGroupSid = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroupSid)) {
        if (!CheckTokenMembership(NULL, adminGroupSid, &isRunAsAdmin)) {
            isRunAsAdmin = FALSE;
        }
        FreeSid(adminGroupSid);
    }
    return isRunAsAdmin;
}

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::trace);

    QApplication a(argc, argv);
    MainWindow w;

    if (IsRunningAsAdmin()) {
        SPDLOG_INFO("Running program as admin");
    } else {
        SPDLOG_INFO("Running program as user");
    }

//    Settings settings;
//    settings.read_from_file();

//    settings.backup_task("W:/backup_testing/1", "scheduled", "18:00");
//    settings.set_destination("W:/backup_testing/destination");
//    settings.push_changes();
//


    auto backup1 = BackupFactory::CreateBackup<ScheduleRecurrence::DAILY>(BackupType::FULL, "W:/backup_testing/source", 25, 30);
    if (backup1 && addTask(*backup1)){
        SPDLOG_INFO("Added backup1");
    } else {
        SPDLOG_INFO("Couldn't add backup1. Error: {}", BackupFactory::ErrorCodeToString(BackupFactory::GetLastError()));
    }
    auto backup2 = BackupFactory::CreateBackup<ScheduleRecurrence::WEEKLY>(BackupType::FULL, "W:/backup_testing/source", 3, 10, 30);
    if (backup2 && addTask(*backup2)){
        SPDLOG_INFO("Added backup2");
    } else {
        SPDLOG_INFO("Couldn't add backup2. Error: {}", BackupFactory::ErrorCodeToString(BackupFactory::GetLastError()));
    }
    auto backup3 = BackupFactory::CreateBackup<ScheduleRecurrence::DAILY>(BackupType::FULL, "/path/to/directory", 10);
    if (backup3 && addTask(*backup3)){
        SPDLOG_INFO("Added backup3");
    } else {
        SPDLOG_INFO("Couldn't add backup3. Error: {}", BackupFactory::ErrorCodeToString(BackupFactory::GetLastError()));
    }
    auto backup4 = BackupFactory::CreateBackup<ScheduleRecurrence::MONTHLY>(BackupType::FULL, "W:/backup_testing/source", -1, 12, 30);
    if (backup4 && addTask(*backup4)){
        SPDLOG_INFO("Added backup4");
    } else {
        SPDLOG_INFO("Couldn't add backup4. Error: {}", BackupFactory::ErrorCodeToString(BackupFactory::GetLastError()));
    }


    // TODO
    /* Duplicate task problems
     *
     *
     *
     */





//    std::thread scheduled_backup_thread(scheduled_backup);

    // Connect the aboutToQuit signal to a lambda that handles shutdown
//    QObject::connect(&a, &QCoreApplication::aboutToQuit, [&]() {
//        shutdown(); // Call your shutdown function here
//        scheduled_backup_thread.join();
//        spdlog::info("Scheduled thread joined");
//    });



    w.show();
    return a.exec();
}
