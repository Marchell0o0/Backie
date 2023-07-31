#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <QApplication>
#include <fstream>
#include <iostream>

#include <condition_variable>
#include <atomic>
#include <thread>
#include <vector>
#include <Windows.h>


#include "spdlog/spdlog.h"

#include "mainwindow.h"
#include "settings.h"

std::atomic<bool> shutdown_requested(false);
std::condition_variable cv;
std::mutex cv_mutex;
std::vector<HANDLE> directory_handles;


std::chrono::system_clock::time_point parse_time(const std::string& time_str) {
    int hour, minute;
    std::sscanf(time_str.c_str(), "%d:%d", &hour, &minute);

    // Get the current time
    auto now = std::chrono::system_clock::now();

    // Convert to time_t
    std::time_t now_tt = std::chrono::system_clock::to_time_t(now);

    // Convert to tm structure
    std::tm now_tm = *std::localtime(&now_tt);

    // Set the desired hour and minute
    now_tm.tm_hour = hour;
    now_tm.tm_min = minute;
    now_tm.tm_sec = 0;

    // Convert back to time_t
    std::time_t desired_tt = std::mktime(&now_tm);

    // Convert to time_point
    std::chrono::system_clock::time_point desired_time_point = std::chrono::system_clock::from_time_t(desired_tt);

    return desired_time_point;
}


void scheduled_backup() {

    Settings settings;
    settings.read_from_file();
    std::vector<Task> scheduled_tasks = settings.get_task_list_by_type("scheduled");

    int i = 0;
    while (!shutdown_requested.load()) {

//        auto next_backup_time = parse_time("22:59");

//        auto next_backup_time = std::chrono::system_clock::now();

//        std::chrono::seconds twenty_seconds(20);

//        next_backup_time += twenty_seconds;

        auto next_backup_time = parse_time(scheduled_tasks[i].time);

        std::unique_lock<std::mutex> lock(cv_mutex);

        if(next_backup_time < std::chrono::system_clock::now()){
            spdlog::info("next backup time already happened");
            return;
        }

        cv.wait_until(lock, next_backup_time, [] { return shutdown_requested.load(); });
        if (!shutdown_requested.load()) {
            spdlog::info("Scheduled backup requested");
        } else {
            spdlog::info("Sceduled thread woken up for joining");
        }
        i += 1;
        i %= scheduled_tasks.size();
    }
}


void shutdown() {
    shutdown_requested.store(true);
    cv.notify_all(); // Wake up the scheduled backup thread

//    // Close the directory handles to unblock ReadDirectoryChangesW
//    for (HANDLE handle : directory_handles) {
//        CloseHandle(handle);
//    }
}


int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::trace);

    QApplication a(argc, argv);
    MainWindow w;

    Settings settings;
    settings.read_from_file();

//    std::vector<Task> scheduled_tasks = settings.get_task_list_by_type("scheduled");

//    for (auto & elem : scheduled_tasks){
//        std::cout << elem.directory;
//        std::cout << elem.type;
//        std::cout << elem.time;
//        std::cout << elem.filter;
//        std::cout << std::endl;
//    }

//    std::vector<Task> watched_tasks = settings.get_task_list_by_type("watched");

//    for (auto & elem : watched_tasks){
//        std::cout << elem.directory;
//        std::cout << elem.type;
//        std::cout << elem.time;
//        std::cout << elem.filter;
//        std::cout << std::endl;
//    }


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



    w.show();
    return a.exec();
}
