#ifndef TEMPLATEBUILDER_H
#define TEMPLATEBUILDER_H

#include "schedule.h"
#include "template.h"

class TemplateBuilder {
public:
	TemplateBuilder& startFrom(const Template& existingTemplate);

	std::optional<Template> build();

	TemplateBuilder& setName(const std::string& name);
	TemplateBuilder& setKey(const std::string& key);
	TemplateBuilder& setActiveTS(bool active);
	TemplateBuilder& setDests(const std::vector<fs::path>& dests);
	TemplateBuilder& setSrcs(const std::vector<fs::path>& srcs);
	TemplateBuilder&
	setSchedules(const std::vector<std::shared_ptr<Schedule>>& schedules);

private:
	std::string name;
	std::string key;
	bool activeTS;
	std::vector<fs::path> dests;
	std::vector<fs::path> srcs;
	std::vector<std::shared_ptr<Schedule>> schedules;
	void clean();
};

#endif // TEMPLATEBUILDER_H
