#include "pch.h"

#include "template.h"

void to_json(json& j, const Template& t) {
	std::vector<std::string> dests_strings;
	std::vector<std::string> srcs_strings;
	std::vector<json> schedules_json;

	for (const auto& d : t.dests) {
		dests_strings.push_back(d.string());
	}
	for (const auto& s : t.srcs) {
		srcs_strings.push_back(s.string());
	}
	for (const auto& sched : t.schedules) {
		schedules_json.push_back(sched ? sched->to_json() : nullptr);
	}

	j = json{{"name", t.name},		   {"key", t.key},
			 {"activeTS", t.activeTS}, {"dests", dests_strings},
			 {"srcs", srcs_strings},   {"schedules", schedules_json}};
	return;
}

void from_json(const json& j, Template& t) {
	std::vector<fs::path> dests;
	std::vector<fs::path> srcs;
	std::vector<std::shared_ptr<Schedule>> schedules;

	for (const auto& d : j.at("dests")) {
		dests.emplace_back(d.get<std::string>());
	}
	for (const auto& s : j.at("srcs")) {
		srcs.emplace_back(s.get<std::string>());
	}
	for (const auto& sched : j.at("schedules")) {
		if (sched.is_null()) {
			schedules.push_back(nullptr);
		} else {
			schedules.push_back(createScheduleFromJson(sched));
		}
	}

	t = Template(j.at("name").get<std::string>(),
				 j.at("key").get<std::string>(), j.at("activeTS").get<bool>(),
				 std::move(dests), std::move(srcs), std::move(schedules));
	return;
}
