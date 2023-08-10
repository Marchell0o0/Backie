#include <atomic>
#include <condition_variable>
#include <vector>

#include <spdlog/spdlog.h>

#include "scheduled_thread.h"
#include "task.h"
#include "settings.h"


std::atomic<bool> shutdown_requested(false);
std::condition_variable cv;
std::mutex cv_mutex;


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

std::chrono::system_clock::time_point next_day(void) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_tt = std::chrono::system_clock::to_time_t(now);
    std::tm next_day_tm = *std::localtime(&now_tt);
    next_day_tm.tm_hour = 0;
    next_day_tm.tm_min = 0;
    next_day_tm.tm_sec = 0;
    next_day_tm.tm_mday += 1; // Increment the day
    std::time_t next_day_tt = std::mktime(&next_day_tm);
    auto next_day = std::chrono::system_clock::from_time_t(next_day_tt);
    return next_day;
}


void scheduled_backup() {
    Settings settings;
    settings.read_from_file();
    std::vector<Task> scheduled_tasks = settings.get_task_list_by_type("scheduled");
    while(!shutdown_requested.load()){

        // Iterate through the tasks, executing all tasks that are scheduled for the same time together
        auto it = scheduled_tasks.begin();
        while (it != scheduled_tasks.end()) {
            auto next_backup_time = parse_time(it->time);

            // Wait for the next scheduled time
            if (next_backup_time > std::chrono::system_clock::now()) {
                std::unique_lock<std::mutex> lock(cv_mutex);
                cv.wait_until(lock, next_backup_time, [] { return shutdown_requested.load(); });
                if (shutdown_requested.load()) {
                    spdlog::info("Scheduled thread woken up for joining");
                    return;
                }

                // Execute all tasks scheduled for the current time
                do {
                    spdlog::info("Scheduled backup requested for task with time {}", it->time);
                    // Execute the backup for *it
                    ++it;
                } while (it != scheduled_tasks.end() && parse_time(it->time) == next_backup_time);


            } else {
                spdlog::info("Task time already happened");
                ++it;
            }
        }
        spdlog::info("Today's tasks have ended, waiting for tomorrow");

        auto next_day_time_point = next_day();

        // Wait until the beginning of the next day, or until shutdown is requested
        std::unique_lock<std::mutex> lock(cv_mutex);
        cv.wait_until(lock, next_day_time_point, [] { return shutdown_requested.load(); });

        if (shutdown_requested.load()) {
            spdlog::info("Scheduled thread woken up for joining");
            return;
        }
        spdlog::info("Starting new day of scheduled tasks");
    }
}


void shutdown() {
    shutdown_requested.store(true);
    cv.notify_all(); // Wake up the scheduled backup thread
}

