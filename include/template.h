#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "schedule.h"

class Template {
	friend class TemplateBuilder;

public:
	Template() = default; // Public default constructor for JSON deserialization

	// Custom serialization and deserialization
	friend void to_json(json& j, const Template& t);
	friend void from_json(const json& j, Template& t);

	const std::string& getName() const { return name; }
	const std::string& getKey() const { return key; }
	const bool getActiveTS() const { return activeTS; }
	const std::vector<fs::path> getDests() const { return dests; }
	const std::vector<fs::path> getSrcs() const { return srcs; }
	const std::vector<std::shared_ptr<Schedule>> getSchedules() const {
		return schedules;
	}
private:
	Template(std::string name, std::string key, bool active,
			 std::vector<fs::path> dests, std::vector<fs::path> srcs,
			 std::vector<std::shared_ptr<Schedule>> schedules)
		: name(std::move(name)), key(std::move(key)), activeTS(active),
		  dests(std::move(dests)), srcs(std::move(srcs)),
		  schedules(std::move(schedules)) {}

	std::string name;
	std::string key;
	bool activeTS;
	std::vector<fs::path> dests;
	std::vector<fs::path> srcs;
	std::vector<std::shared_ptr<Schedule>> schedules;
};

#endif // TEMPLATE_H
