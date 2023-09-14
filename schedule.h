#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "nlohmann/json.hpp"

enum class ScheduleRecurrence {
    DAILY,
    WEEKLY,
    MONTHLY,
    ONCE,
};

class Schedule {
public:
    virtual ~Schedule() {}
    virtual nlohmann::json toJson() const = 0;
    virtual ScheduleRecurrence getRecurrence() const = 0;
    virtual bool verify() const =0;
//    BackupType type;
};

class OnceSchedule : public Schedule {
public:
    int year;
    int month;
    int day;
    int hour;
    int minute;
    nlohmann::json toJson() const override;
    bool verify() const override;
    ScheduleRecurrence getRecurrence() const override { return this->recurrence; }
private:
    ScheduleRecurrence recurrence = ScheduleRecurrence::ONCE;
};

class MonthlySchedule : public Schedule {
public:
    int day;
    int hour;
    int minute;
    nlohmann::json toJson() const override;
    bool verify() const override;
    ScheduleRecurrence getRecurrence() const override { return this->recurrence; }
private:
    ScheduleRecurrence recurrence = ScheduleRecurrence::MONTHLY;
};

class WeeklySchedule : public Schedule {
public:
    int day;
    int hour;
    int minute;
    nlohmann::json toJson() const override;
    bool verify() const override;
    ScheduleRecurrence getRecurrence() const override { return this->recurrence; }
private:
    ScheduleRecurrence recurrence = ScheduleRecurrence::WEEKLY;
};

class DailySchedule : public Schedule {
public:
    int hour;
    int minute;
    nlohmann::json toJson() const override;
    bool verify() const override;
    ScheduleRecurrence getRecurrence() const override { return this->recurrence; }
private:
    ScheduleRecurrence recurrence = ScheduleRecurrence::DAILY;
};

#endif // SCHEDULE_H
