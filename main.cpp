#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <QApplication>
#include <QPushButton>
#include <QLocalServer>
#include <QLocalSocket>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
#include <windows.h>

#include "spdlog/spdlog.h"

#include "mainwindow.h"

#include "backuperrand.h"
#include "utils.h"
#include "settings.h"
#include "destination.h"
#include "backupbuilder.h"


int backupWorker(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    QLocalSocket socket;
    socket.connectToServer("BackupServer");

    bool connected = false;
    if (socket.waitForConnected(1000)) {  // 1 second timeout, adjust as needed
        connected = true;
    }

    if (argc != 4) {
        if (connected) { socket.write("Wrong number of arguments"); }
        return 1;
    }

//    std::string name = argv[2];
//    BackupType type = typeFromStr(argv[3]);

//    Settings& settings = Settings::getInstance();
//    BackupBuilder builder;
//    auto backup_errand = builder
//                            .setCurrentType(type)
//                            .setDestinations(settings.getBackupDests(name))
//                            .setSources(settings.getBackupSrcs(name))
//                            .buildErrand();

//    if (backup_errand) {
//        backup_errand->performBackup();
//    } else {
//        if (connected) { socket.write("Couldn't create backup errand"); }
//        return 1;
//    }

    return 0;
}

int guiMain(int argc, char* argv[]) {
    SPDLOG_INFO("Drawing gui...");

    QLocalServer server;
    if(!server.listen("BackupServer")) {
        SPDLOG_ERROR("Server is not listening");
    }

    QObject::connect(&server, &QLocalServer::newConnection, &server, [&]() {
        QLocalSocket* clientConnection = server.nextPendingConnection();
        QObject::connect(clientConnection, &QLocalSocket::disconnected,
                         clientConnection, &QLocalSocket::deleteLater);

        QObject::connect(clientConnection, &QLocalSocket::readyRead, [clientConnection]() {
            // read the data sent by the backup instance
            QByteArray data = clientConnection->readAll();
            std::cout << "Received data: " << data.data() << std::endl;
        });
    });

    QApplication app(argc, argv);
    MainWindow mainWindow;

//    if (IsRunningAsAdmin()){
//        SPDLOG_INFO("Running with admin rights");
//    } else {
//        SPDLOG_INFO("Running without admin rights");
//    }

//    Settings& settings = Settings::getInstance();

//    Destination test_dest1 = { .name = "Default destination", .destinationFolder = "W:/Backie backups/Dest 1"};
//    settings.addUpdateDest(test_dest1);

//    Destination test_dest2 = { .name = "Default destination 2", .destinationFolder = "W:/Backie backups/Dest 2"};
//    settings.addUpdateDest(test_dest2);

//    std::shared_ptr<OnceBackupSchedule> once = std::make_shared<OnceBackupSchedule>();
//    once->type = BackupType::FULL;
//    once->year = 2020;
//    once->month = 11;
//    once->day = 20;
//    once->hour = 12;
//    once->minute = 35;

//    std::shared_ptr<MonthlyBackupSchedule> monthly = std::make_shared<MonthlyBackupSchedule>();
//    monthly->type = BackupType::FULL;
//    monthly->day = 20;
//    monthly->hour = 9;
//    monthly->minute = 0;

//    std::shared_ptr<WeeklyBackupSchedule> weekly = std::make_shared<WeeklyBackupSchedule>();
//    weekly->type = BackupType::INCREMENTAL;
//    weekly->day = 5;
//    weekly->hour = 23;
//    weekly->minute = 59;

//    std::shared_ptr<DailyBackupSchedule> daily = std::make_shared<DailyBackupSchedule>();
//    daily->type = BackupType::INCREMENTAL;
//    daily->hour = 0;
//    daily->minute = 0;

//    BackupBuilder builder;

//    auto test_backup1 = builder
//                            .setName("Minecraft")
//                            .setDestinations({test_dest1.name, test_dest2.name})
//                            .setSources({"W:\\Src folder 1"})
//                            .setSchedules({weekly, daily})
//                            .buildSchedule();
//    auto test_backup2 = builder
//                            .setName("Homework")
//                            .setDestinations({test_dest2.name})
//                            .setSources({"W:\\Src folder 2"})
//                            .setSchedules({once})
//                            .buildSchedule();
//    auto test_backup3 = builder
//                            .setName("Saves")
//                            .setDestinations({test_dest1.name})
//                            .setSources({"W:\\Src folder 3"})
//                            .setSchedules({monthly})
//                            .buildSchedule();
//    auto test_backup4 = builder
//                            .setName("Minecraft 2")
//                            .setDestinations({test_dest1.name, test_dest2.name})
//                            .setSources({"W:\\Src folder 1"})
//                            .setSchedules({daily})
//                            .buildSchedule();

//    settings.addUpdateBackup(*test_backup1);
//    settings.addUpdateBackup(*test_backup2);
//    settings.addUpdateBackup(*test_backup3);
//    settings.addUpdateBackup(*test_backup4);



//    std::vector<Backup> backups = settings.getBackupVec();

//    for (Backup backup : backups) {
//        SPDLOG_INFO("Name: {}, current type: {}", backup.getName(), strFromType(backup.getCurrentType()));
//        std::cout << "Destinations:" << std::endl;
//        for (auto& destinationName : backup.getDestinations()) {
//            std::cout << destinationName << std::endl;
//        }
//        std::cout << "Sources:" << std::endl;
//        for (fs::path& source : backup.getSources()) {
//            std::cout << source.string() << std::endl;
//        }
//        std::cout << "Schedule types:" << std::endl;
//        for (auto& schedule : backup.getSchedules()) {
//            std::cout << strFromType(schedule->type) << std::endl;
//        }
//    }


//    if (backups.size() > 0) {
//        auto changed_backup = builder
//                                        .setChanging(backups[0])
////                                        .setName("Minecraft")
//                                        .setDestinations({test_dest})
//                                        .setSources({"W:\\Src folder 1"})
//                                        .setSchedules({once})
//                                        .buildSchedule();

//        if (changed_backup) {
//            backups[0] = *changed_backup;
//            SPDLOG_INFO("Created a test schedule backup");
//        } else {
//            SPDLOG_ERROR("Couldn't create test schedule backup");
//        }
//    }


//    auto test_errand_backup = builder
//                                  .setName("Minecraft")
//                                  .setCurrentType(BackupType::FULL)
//                                  .buildErrand();

//    if (test_errand_backup) {
//        SPDLOG_INFO("Created a test errand backup");
//    } else {
//        SPDLOG_ERROR("Couldn't create test errand backup");
//    }

    // Destination management
    /*
    Destination test_dest = { .name = "Default destination", .destinationFolder = "W:/Backie backups/Dest 1"};
    settings.addUpdateDest(test_dest);
    Destination test_dest2 = { .name = "Default destination 2", .destinationFolder = "W:/Backie backups/Dest 2"};
    settings.addUpdateDest(test_dest2);

    std::vector<Destination> destVec = settings.getDestVec();
    for (auto it : destVec){
        SPDLOG_INFO("{} {}", it.name, it.destinationFolder.string());
    }

    settings.removeDest(test_dest);
    settings.removeDest(test_dest2);
    */

    mainWindow.show();
    return app.exec();
}

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::trace);

    Settings& settings = Settings::getInstance();
    if (!settings.initializeSettings()) {
        SPDLOG_ERROR("Couldn't create/read settings.json");
        exit(1);
    }

    if (argc > 1 && strcmp(argv[1], "--backup") == 0) {
        return backupWorker(argc, argv);
    } else {
        return guiMain(argc, argv);
    }
}
