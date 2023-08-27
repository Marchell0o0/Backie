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

#include "backup.h"
//#include "backupschedule.h"
#include "backupfactory.h"

#include "utils.h"

//#include "settings.h"


int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::trace);

    QApplication a(argc, argv);
    MainWindow mainWindow;

//    Settings settings;
//    settings.read_from_file();
    if (argc > 1 && strcmp(argv[1], "--backup") == 0) {
        QLocalSocket socket;
        socket.connectToServer("BackupServer");

        bool connected = false;
        if (socket.waitForConnected(1000)) {  // 1 second timeout, adjust as needed
            connected = true;
        }

        QCoreApplication app(argc, argv);

        if (argc != 4) {
//            SPDLOG_ERROR("Wrong number of arguments");
            if (connected){
                socket.write("Wrong number of arguments");
            }
            return 1;
        }

        std::filesystem::path directory = argv[2];

        BackupType type = typeFromStr(argv[3]);

        auto backup = BackupFactory::CreateBackup(type, directory);
        if (!backup) {
            if (connected){
                std::string error = BackupFactory::ErrorCodeToString(BackupFactory::GetLastCreationError());
                std::string message = "Couldn't create backup. Error: " + error;
                socket.write(message.c_str());
            }
            return 1;
        }

        if (connected){
            std::string typeStr = std::to_string(static_cast<int>(backup->getType())); // Convert type to string representation
            std::string message = "Backup requested with type " + typeStr + " for " + directory.u8string();
            socket.write(message.c_str());
        }

        sleep(1);

//        backup->performBackup();

        if (connected){
            socket.write("Backup executed succefuly");
        }

        return 0;
    } else {
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

        // Backlup Schedule testing
        /*
        auto backupSchedule_test = BackupFactory::CreateBackupSchedule<ScheduleRecurrence::MONTHLY>(BackupType::INCREMENTAL, "W:/backup_testing/source", -1, 10, 30);
        if (!backupSchedule_test){
            SPDLOG_ERROR("Couldn't create backupShedule_test. Error: {}",
                         BackupFactory::ErrorCodeToString(BackupFactory::GetLastCreationError()));
        } else {
            HRESULT hr = backupSchedule_test->addToTaskScheduler();
            if (FAILED(hr)){
                SPDLOG_ERROR("Couldn't add backupSchedule_test task. Error code: {}", hr);
            }
        }
        */

        // Full backup testing
        /*
        auto backup_test_full = BackupFactory::CreateBackup(BackupType::FULL, "W:\\Src folder 1");
        if(!backup_test_full){
            SPDLOG_ERROR("Couldn't create test backup object. Error: {}", BackupFactory::ErrorCodeToString(BackupFactory::GetLastCreationError()));
        } else {
            SPDLOG_INFO("Performing full backup...");
            if (!backup_test_full->performBackup();) {
                SPDLOG_ERROR("Couldn't perform the backup");
            }
        }
        */

        // Incremental backup testing
        /*
        auto backup_test_incremental = BackupFactory::CreateBackup(BackupType::INCREMENTAL, "W:\\Src folder 1");
        if(!backup_test_incremental){
            SPDLOG_ERROR("Couldn't create test backup object. Error: {}", BackupFactory::ErrorCodeToString(BackupFactory::GetLastCreationError()));
        } else {
            SPDLOG_INFO("Performing incremental backup...");
            if (!backup_test_incremental->performBackup()) {
                SPDLOG_ERROR("Couldn't perform the backup");
            }
        }
        */

        SPDLOG_INFO("Drawing gui...");

        mainWindow.show();
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
