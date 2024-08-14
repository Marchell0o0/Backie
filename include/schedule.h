#ifndef SCHEDULE_H
#define SCHEDULE_H

enum class ScheduleRecurrence { DAILY, WEEKLY, MONTHLY, ONCE, UNKNOWN };

class Schedule {
public:
    virtual ~Schedule() {}
	virtual nlohmann::json to_json() const = 0;
	virtual ScheduleRecurrence getRecurrence() const = 0;
	virtual bool verify() const = 0;
};

std::unique_ptr<Schedule> createScheduleFromJson(const nlohmann::json& j);

class OnceSchedule : public Schedule {
public:
    int year;
    int month;
    int day;
    int hour;
    int minute;
	nlohmann::json to_json() const override;
	bool verify() const override;
	ScheduleRecurrence getRecurrence() const override {
		return this->recurrence;
	}

private:
    ScheduleRecurrence recurrence = ScheduleRecurrence::ONCE;
};

class MonthlySchedule : public Schedule {
public:
    int day;
    int hour;
    int minute;
	nlohmann::json to_json() const override;
	bool verify() const override;
	ScheduleRecurrence getRecurrence() const override {
		return this->recurrence;
	}

private:
    ScheduleRecurrence recurrence = ScheduleRecurrence::MONTHLY;
};

class WeeklySchedule : public Schedule {
public:
    int day;
    int hour;
    int minute;
	nlohmann::json to_json() const override;
	bool verify() const override;
	ScheduleRecurrence getRecurrence() const override {
		return this->recurrence;
	}

private:
    ScheduleRecurrence recurrence = ScheduleRecurrence::WEEKLY;
};

class DailySchedule : public Schedule {
public:
    int hour;
    int minute;
	nlohmann::json to_json() const override;
	bool verify() const override;
	ScheduleRecurrence getRecurrence() const override {
		return this->recurrence;
	}

private:
    ScheduleRecurrence recurrence = ScheduleRecurrence::DAILY;
};

#endif // SCHEDULE_H
