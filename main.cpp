#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <QApplication>
#include <QPushButton>
#include <QLocalServer>
#include <QLocalSocket>
#include <fstream>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <windows.h>

#include "spdlog/spdlog.h"

#include "mainwindow.h"

#include "errand.h"
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

    std::string key = argv[2];
    BackupType type = typeFromStr(argv[3]);

    BackupBuilder builder;
    auto errand = builder
                            .setKey(key)
                            .setCurrentType(type)
                            .buildErrand();

    if (errand) {
        errand->perform();
    } else {
        if (connected) { socket.write("Couldn't create backup errand"); }
        return 1;
    }

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


    BackupBuilder builder;
    auto errand = builder
                      .setKey("1d16ce4f-e996-429b-a3c9-bcb1222f1d14")
                      .setCurrentType(BackupType::FULL)
                      .buildErrand();

    if (errand) {
        errand->perform();
        SPDLOG_INFO("Performed a backup");
    } else {
        SPDLOG_ERROR("Error");
    }

    /*

    Settings& settings = Settings::getInstance();

    SPDLOG_INFO("Deleting tasks");
    // delete all tasks
    for (auto& task : settings.getTaskVec()) {
        task.deleteLocal();
    }
    SPDLOG_INFO("Deleting destinations");
    // delete all destinations
    for (auto& dest : settings.getDestVec()) {
        settings.remove(dest);
    }

    Destination test_dest1("Default destination 1", "W:\\Backie backups\\Dest 1");
    settings.addUpdate(test_dest1);

    Destination test_dest2("Default destination 2", "W:\\Backie backups\\Dest 2");
    settings.addUpdate(test_dest2);

    std::shared_ptr<OnceSchedule> once = std::make_shared<OnceSchedule>();
    once->type = BackupType::FULL;
    once->year = 2020;
    once->month = 11;
    once->day = 20;
    once->hour = 12;
    once->minute = 35;

    std::shared_ptr<MonthlySchedule> monthly = std::make_shared<MonthlySchedule>();
    monthly->type = BackupType::FULL;
    monthly->day = 20;
    monthly->hour = 9;
    monthly->minute = 0;

    std::shared_ptr<WeeklySchedule> weekly = std::make_shared<WeeklySchedule>();
    weekly->type = BackupType::INCREMENTAL;
    weekly->day = 5;
    weekly->hour = 23;
    weekly->minute = 59;

    std::shared_ptr<DailySchedule> daily = std::make_shared<DailySchedule>();
    daily->type = BackupType::INCREMENTAL;
    daily->hour = 0;
    daily->minute = 0;

    BackupBuilder builder;
    auto test_task1 = builder
                            .setName("Minecraft")
                            .setDestinations({test_dest1, test_dest2})
                            .setSources({"W:\\Src folder 1"})
                            .setSchedules({weekly, daily})
                            .buildTask();
    auto test_task2 = builder
                            .setName("Homework")
                            .setDestinations({test_dest2})
                            .setSources({"W:\\Src folder 2"})
                            .setSchedules({once})
                            .buildTask();
    auto test_task3 = builder
                            .setName("Saves")
                            .setDestinations({test_dest1})
                            .setSources({"W:\\Src folder 3"})
                            .setSchedules({monthly})
                            .buildTask();
    auto test_task4 = builder
                            .setName("Minecraft 2")
                            .setDestinations({test_dest1, test_dest2})
                            .setSources({"W:\\Src folder 1"})
                            .setSchedules({daily})
                            .buildTask();

    test_task1->saveLocal();
    test_task2->saveLocal();
    test_task3->saveLocal();
    test_task4->saveLocal();


    std::vector<Task> tasks = settings.getTaskVec();
    std::vector<Destination> dests = settings.getDestVec();

    std::cout << "Tasks:" << std::endl;
    for (auto& task : tasks) {
        std::cout << task << std::endl;
    }

    std::cout << "Global destinations:" << std::endl;
    for (auto& dest : dests) {
        std::cout << dest << std::endl;
    }
    */

    //    if (IsRunningAsAdmin()){
    //        SPDLOG_INFO("Running with admin rights");
    //    } else {
    //        SPDLOG_INFO("Running without admin rights");
    //    }


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
