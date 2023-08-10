#ifndef SCHEDULED_THREAD_H
#define SCHEDULED_THREAD_H




#include <string>
#include <chrono>
std::chrono::system_clock::time_point parse_time(const std::string& time_str);

std::chrono::system_clock::time_point next_day(void);

void scheduled_backup();

void shutdown();

#endif // SCHEDULED_THREAD_H
