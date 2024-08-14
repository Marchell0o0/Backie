#include "pch.h"

#include "templatebuilder.h"
#include "utils.h"

TemplateBuilder& TemplateBuilder::setName(const std::string& name) {
	this->name = name;
	return *this;
}

TemplateBuilder&
TemplateBuilder::TemplateBuilder::setKey(const std::string& key) {
	this->key = key;
	return *this;
}

TemplateBuilder& TemplateBuilder::setActiveTS(bool active) {
	this->activeTS = active;
	return *this;
}

TemplateBuilder& TemplateBuilder::setDests(const std::vector<fs::path>& dests) {
	this->dests = dests;
	return *this;
}

TemplateBuilder& TemplateBuilder::setSrcs(const std::vector<fs::path>& srcs) {
	this->srcs = srcs;
	return *this;
}

TemplateBuilder& TemplateBuilder::setSchedules(
	const std::vector<std::shared_ptr<Schedule>>& schedules) {
	this->schedules = schedules;
	return *this;
}

TemplateBuilder& TemplateBuilder::startFrom(const Template& existingTemplate) {
	name = existingTemplate.getName();
	key = existingTemplate.getKey();
	activeTS = existingTemplate.getActiveTS();
	dests = existingTemplate.getDests();
	srcs = existingTemplate.getSrcs();
	schedules = existingTemplate.getSchedules();
	return *this;
}

void TemplateBuilder::clean() {
	name.clear();
	key.clear();
	activeTS = false;
	dests.clear();
	srcs.clear();
	schedules.clear();
}

std::optional<Template> TemplateBuilder::build() {
	// TODO: all the wrong input checks
	if (name.empty()) {
		SPDLOG_WARN("BUILDER: Name can't be empty");
		return std::nullopt;
	}

	if (activeTS && schedules.empty()) {
		SPDLOG_WARN("BUILDER: Can't active in TS without schedules");
		return std::nullopt;
	}
	if (dests.empty() || srcs.empty()) {
		SPDLOG_WARN("BUILDER: Destinations or sources can't be empty");
		return std::nullopt;
	}

	for (const auto& schedule : schedules) {
		if (!schedule->verify()) {
			SPDLOG_WARN("BUILDER: At least one of the schedules is wrong");
			return std::nullopt;
		}
	}
	Template temp(name, key.empty() ? generate_uuid_v4() : key, activeTS, dests,
				  srcs, schedules);
	clean();
	return temp;
}
