#include "pch.h"

#include "schedule.h"

std::string recurrenceToString(ScheduleRecurrence recurrence) {
	switch (recurrence) {
	case ScheduleRecurrence::DAILY:
		return "DAILY";
	case ScheduleRecurrence::WEEKLY:
		return "WEEKLY";
	case ScheduleRecurrence::MONTHLY:
		return "MONTHLY";
	case ScheduleRecurrence::ONCE:
		return "ONCE";
	default:
		return "UNKNOWN";
	}
}

ScheduleRecurrence stringToRecurrence(const std::string& str) {
	if (str == "DAILY")
		return ScheduleRecurrence::DAILY;
	if (str == "WEEKLY")
		return ScheduleRecurrence::WEEKLY;
	if (str == "MONTHLY")
		return ScheduleRecurrence::MONTHLY;
	if (str == "ONCE")
		return ScheduleRecurrence::ONCE;
	else {
		return ScheduleRecurrence::UNKNOWN;
	}
}

// The factory function
std::unique_ptr<Schedule> createScheduleFromJson(const nlohmann::json& j) {
	ScheduleRecurrence recurrence
		= stringToRecurrence(j.at("recurrence").get<std::string>());
	switch (recurrence) {
	case (ScheduleRecurrence::ONCE): {
		std::unique_ptr<OnceSchedule> obj = std::make_unique<OnceSchedule>();
		obj->year = j.at("year").get<int>();
		obj->month = j.at("month").get<int>();
		obj->day = j.at("day").get<int>();
		obj->hour = j.at("hour").get<int>();
		obj->minute = j.at("minute").get<int>();
		return obj;
	}
	case (ScheduleRecurrence::MONTHLY): {
		std::unique_ptr<MonthlySchedule> obj
			= std::make_unique<MonthlySchedule>();
		obj->day = j.at("day").get<int>();
		obj->hour = j.at("hour").get<int>();
		obj->minute = j.at("minute").get<int>();
		return obj;
	}
	case (ScheduleRecurrence::WEEKLY): {
		std::unique_ptr<WeeklySchedule> obj
			= std::make_unique<WeeklySchedule>();
		obj->day = j.at("day").get<int>();
		obj->hour = j.at("hour").get<int>();
		obj->minute = j.at("minute").get<int>();
		return obj;
	}
	case (ScheduleRecurrence::DAILY): {
		std::unique_ptr<DailySchedule> obj = std::make_unique<DailySchedule>();
		obj->hour = j.at("hour").get<int>();
		obj->minute = j.at("minute").get<int>();
		return obj;
	}
	default:
		// TODO: chatgpt code
		throw std::runtime_error("Unknown Schedule type in JSON");
	}
}

json OnceSchedule::to_json() const {
	return json{{"recurrence", recurrenceToString(recurrence)},
				{"year", year},
				{"month", month},
				{"day", day},
				{"hour", hour},
				{"minute", minute}};
}

json MonthlySchedule::to_json() const {
	return json{{"recurrence", recurrenceToString(recurrence)},
				{"day", day},
				{"hour", hour},
				{"minute", minute}};
}

json WeeklySchedule::to_json() const {
	return json{{"recurrence", recurrenceToString(recurrence)},
				{"day", day},
				{"hour", hour},
				{"minute", minute}};
}

json DailySchedule::to_json() const {
	return json{{"recurrence", recurrenceToString(recurrence)},
				{"hour", hour},
				{"minute", minute}};
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

bool MonthlySchedule::verify() const {
    return !(hour < 0 || hour > 23 || minute < 0 || minute > 59 || day > 31)
           && (day == -1 || day > 0);
}

bool WeeklySchedule::verify() const  {
    return !(hour < 0 || hour > 23 || minute < 0 || minute > 59 ||
            day < 1 || day > 7);
}

bool DailySchedule::verify() const {
    return !(hour < 0 || hour > 23 || minute < 0 || minute > 59);
}
