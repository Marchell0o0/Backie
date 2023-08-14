#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <QApplication>
#include <QPushButton>
#include <fstream>
#include <iostream>

#include <thread>
#include <vector>

//#include <Windows.h>


#include "spdlog/spdlog.h"

#include "scheduled_thread.h"

#include "mainwindow.h"
#include "settings.h"


int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::trace);

    QApplication a(argc, argv);
    MainWindow mainWindow;

//    Settings settings;
//    settings.read_from_file();

//    settings.backup_task("W:/backup_testing/1", "scheduled", "18:00");
//    settings.set_destination("W:/backup_testing/destination");
//    settings.push_changes();


    std::thread scheduled_backup_thread(scheduled_backup);

    // Connect the aboutToQuit signal to a lambda that handles shutdown
    QObject::connect(&a, &QCoreApplication::aboutToQuit, [&]() {
        shutdown(); // Call your shutdown function here
        scheduled_backup_thread.join();
        spdlog::info("Scheduled thread joined");
    });



    mainWindow.show();
    return a.exec();
}
