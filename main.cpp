#include "mainwindow.h"

#include <QApplication>
#include <fstream>
#include <iostream>


#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE


#include "spdlog/spdlog.h"

#include "settings.h"


class BackupEngine {

};


int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::trace);

    QApplication a(argc, argv);
    MainWindow w;

    Settings settings;

    settings.read_from_file();

    settings.backup_task("W:/backup_testing/1", "scheduled", "18:00");

    settings.backup_task("W:/backup_testing/2", "scheduled", "8:00");

    settings.set_destination("W:/backup_testing/destination");

    settings.push_changes();

    w.show();
    return a.exec();
}
