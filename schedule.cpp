#include "schedule.h"

nlohmann::json OnceSchedule::toJson() const {
    return nlohmann::json{
        {"recurrence", recurrence},
//        {"type", type},
        {"year", year},
        {"month", month},
        {"day", day},
        {"hour", hour},
        {"minute", minute}
    };
}

bool OnceSchedule::verify() const {
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 ||
        day < 1 || day > 31 || month < 1 || month > 12 ||
        year < 1900 || year > 9999) {
        return false;
    }
    switch (month) {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        return day<=31;
    case 4: case 6: case 9: case 11:
        return day <=30;
    case 2:
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            return day <= 29;
        }
        else {
            return day <= 28;
        }
    }
    return true;
}

nlohmann::json MonthlySchedule::toJson() const {
    return nlohmann::json{
        {"recurrence", recurrence},
//        {"type", type},
        {"day", day},
        {"hour", hour},
        {"minute", minute}
    };
}
bool MonthlySchedule::verify() const {
    return !(hour < 0 || hour > 23 || minute < 0 || minute > 59 || day > 31)
           && (day == -1 || day > 0);
}

nlohmann::json WeeklySchedule::toJson() const  {
    return nlohmann::json{
        {"recurrence", recurrence},
//        {"type", type},
        {"day", day},
        {"hour", hour},
        {"minute", minute}
    };
}
bool WeeklySchedule::verify() const  {
    return !(hour < 0 || hour > 23 || minute < 0 || minute > 59 ||
            day < 1 || day > 7);
}


nlohmann::json DailySchedule::toJson() const {
    return nlohmann::json{
        {"recurrence", recurrence},
//        {"type", type},
        {"hour", hour},
        {"minute", minute}
    };
}
bool DailySchedule::verify() const {
    return !(hour < 0 || hour > 23 || minute < 0 || minute > 59);
}
