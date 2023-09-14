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

    if (argc != 3) {
        if (connected) { socket.write("Wrong number of arguments"); }
        return 1;
    }

    std::string key = argv[2];
//    BackupType type = typeFromStr(argv[3]);

    BackupBuilder builder;
    auto errand = builder
                            .setKey(key)
//                            .setCurrentType(type)
                            .buildErrand();

    if (errand) {
        if (connected && errand->perform())
        { socket.write("Performed a backup"); }
    } else {
        if (connected) { socket.write("Couldn't create backup errand"); }
        return 1;
    }

    return 0;
}

namespace Test {
    void cleanSettings() {
        Settings& settings = Settings::getInstance();

        // delete all tasks
        for (auto& task : settings.getTaskVec()) {
//            SPDLOG_INFO("Removing task with key: {}", task.getKey());
            task.deleteLocal();
        }
        // delete all destinations
        for (auto& dest : settings.getDestVec()) {
//            SPDLOG_INFO("Removing dest with key: {}", dessssazst.getKey());
            settings.remove(dest);
        }
    }
    void populateSettings() {
        Settings& settings = Settings::getInstance();
        Destination test_dest1("Default destination 1", "W:\\Backie backups\\Dest 1");
        settings.addUpdate(test_dest1);

        Destination test_dest2("Default destination 2", "W:\\Backie backups\\Dest 2");
        settings.addUpdate(test_dest2);

        std::shared_ptr<OnceSchedule> once = std::make_shared<OnceSchedule>();
        once->year = 2020;
        once->month = 11;
        once->day = 20;
        once->hour = 12;
        once->minute = 35;

        std::shared_ptr<MonthlySchedule> monthly = std::make_shared<MonthlySchedule>();
        monthly->day = 20;
        monthly->hour = 9;
        monthly->minute = 0;

        std::shared_ptr<WeeklySchedule> weekly = std::make_shared<WeeklySchedule>();
        weekly->day = 5;
        weekly->hour = 23;
        weekly->minute = 59;

        std::shared_ptr<DailySchedule> daily = std::make_shared<DailySchedule>();
        daily->hour = 0;
        daily->minute = 0;

        std::shared_ptr<MonthlySchedule> monthlyTest = std::make_shared<MonthlySchedule>();
        monthlyTest->day = 31;
        monthlyTest->hour = 9;
        monthlyTest->minute = 0;

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
        auto test_task5 = builder
                              .setName("Minecraft 3")
                              .setDestinations({test_dest1, test_dest2})
                              .setSources({"W:\\Src folder 1"})
                              .setSchedules({monthlyTest})
                              .buildTask();

        if (test_task1) {
            test_task1->saveLocal();
        }
        test_task2->saveLocal();
        test_task3->saveLocal();
        test_task4->saveLocal();
        test_task5->saveLocal();
    }

    void getPrintSettings() {
        Settings& settings = Settings::getInstance();
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
    }
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



//    BackupFolder folder;
//    folder.parentId;
//    folder.folder;
//    folder.type;
//    while (parentId != "");

//    std::map<std::string backupName, std::vector<BackupFolder>> backupsMap;

//    //TODO: key in backup folder, not name;
//    settings.setLatestId(key, folderIn);

//    recoveryOrigin(pairIn);



//    Test::cleanSettings();

//    //    Test::populateSettings();

//    //    Test::getPrintSettings();


//    //TODO: check what happens if you bring back a deleted file
//    Settings& settings = Settings::getInstance();
////    if (settings.getTaskVec().empty()){
////        SPDLOG_INFO("Task vector is empty, creating new task");
//        const std::time_t now = time(0);
//        const std::tm time = *std::localtime(std::addressof(now));

//        std::shared_ptr<OnceSchedule> onceFull = std::make_shared<OnceSchedule>();
//        onceFull->year = 2023;
//        onceFull->month = time.tm_mon + 1;
//        onceFull->day = time.tm_mday;
//        onceFull->hour = time.tm_hour;
//        onceFull->minute = time.tm_min + 2;

////        std::shared_ptr<OnceSchedule> onceIncremental = std::make_shared<OnceSchedule>();
////        onceIncremental->year = 2023;
////        onceIncremental->month = 9;
////        onceIncremental->day = 10;
////        onceIncremental->hour = time.tm_hour;
////        onceIncremental->minute = time.tm_min + 2;

////        Destination test_dest1("Default destination 1", "W:\\Backie backups\\Dest 1");
//        Destination test_dest1("Default destination 1", "D:\\Code\\sidebaricons");
//        settings.addUpdate(test_dest1);

//        BackupBuilder builder;
//        auto test_task = builder
//                        .setName("Current Dest 2")
//                        .setSchedules({onceFull})
//                        .setDestinations({test_dest1})
//                        .setSources({"D:\\Gallery\\backgrounds"})
//                        .buildTask();

//        if (test_task) {
//        test_task->saveLocal();

//        } else {
//        qDebug() << "Didnt perform saveLocal";
//        }

//        std::vector<Task> tasks = settings.getTaskVec();
//        std::vector<Destination> dests = settings.getDestVec();

//        std::cout << "Tasks:" << std::endl;
//        for (auto& task : tasks) {
//        std::cout << task << std::endl;
//        }

//        std::cout << "Global destinations:" << std::endl;
//        for (auto& dest : dests) {
//        std::cout << dest << std::endl;
//        }
//    }

//    std::vector<Task> tasks = settings.getTaskVec();
//        test_task->perform();
//    if (!tasks.empty()) {
//        if (argc > 1) {
////            tasks[0].setCurrentType(static_cast<BackupType>(atoi(argv[1])));
//            tasks[0].perform();
//        }
//    } else {
//        SPDLOG_ERROR("Couldn't get the task");
//    }

    mainWindow.show();
    return app.exec();
}

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::trace);

    //    if (IsRunningAsAdmin()){
    //        SPDLOG_INFO("Running with admin rights");
    //    } else {
    //        SPDLOG_INFO("Running without admin rights");
    //    }

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
