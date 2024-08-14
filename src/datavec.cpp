#include "pch.h"

#include "datavec.h"
#include "taskscheduler.h"

// default save and init functions for datavecs
template <typename T> bool DataVec<T>::readVec(fs::path &path) {

	this->path = path;
	std::string fileName = path.filename().string();

	std::ifstream file(path);
	if (!file.is_open()) {
		SPDLOG_DEBUG("Creating new {}", fileName);
		std::ofstream fout(path);
		if (fout.is_open()) {
			fout << "[]";
			fout.close();
		} else {
			SPDLOG_ERROR("Could not create new {}", fileName);
			return false;
		}
	} else {
		SPDLOG_DEBUG("{} already exists", fileName);
		json j;
		file >> j;
		for (const auto &item : j) {
			vec.push_back(item.get<T>());
		}
	}
	return true;
}

// TODO: optimization: saves the whole vector on every save
template <typename T> bool DataVec<T>::saveVec() {
	std::ofstream file(path);
	if (!file.is_open()) {
		std::cerr << "Error opening file: " << path << std::endl;
		return false;
	}

	json j = vec;
	file << j.dump(4);
	return true;
}

std::vector<Template>::iterator
TemplateVec::erase(std::vector<Template>::iterator pos) {
	TaskScheduler &TS = TaskScheduler::getInstance();
	if (!TS.getIsInitialized()) {
		SPDLOG_ERROR("Task scheduler isn't initialized");
		return pos;
	}

	if (pos->getActiveTS()) {
		if (!TS.erase(pos->getKey())) {
			SPDLOG_WARN("Couldn't remove taks from task scheduler. Key: {}",
						pos->getKey());
		}
	}
	return DataVec::erase(pos);
}

std::vector<Template>::iterator
TemplateVec::erase(std::vector<Template>::iterator first,
				   std::vector<Template>::iterator last) {
	TaskScheduler &TS = TaskScheduler::getInstance();
	if (!TS.getIsInitialized()) {
		SPDLOG_ERROR("Task scheduler isn't initialized");
		return first;
	}

	for (auto it = first; it != last; ++it) {
		if (!it->getActiveTS()) {
			continue;
		}
		if (!TS.erase(it->getKey())) {
			SPDLOG_WARN("Couldn't remove taks from task scheduler. Key: {}",
						it->getKey());
		}
	}
	return DataVec::erase(first, last);
}

void TemplateVec::change(std::vector<Template>::iterator pos,
						 const Template &newObject) {

	TaskScheduler &TS = TaskScheduler::getInstance();
	if (!TS.getIsInitialized()) {
		SPDLOG_ERROR("Task scheduler isn't initialized");
		return;
	}

	if (newObject.getKey() != pos->getKey()) {
		SPDLOG_WARN("Trying to change a template with a different key. Use "
					"push_back()");
		return;
	}

	if (newObject.getActiveTS()) {
		if (!TS.change(newObject)) {
			SPDLOG_WARN("Couldn't change a task in task scheduler. Key: {}",
						newObject.getKey());
		}
	} else if (!newObject.getActiveTS() && TS.exists(newObject.getKey())) {
		SPDLOG_DEBUG("Template changed to non-active, removing from TS");
		if (!TS.erase(newObject.getKey())) {
			SPDLOG_WARN("Couldn't remove taks from task scheduler. Key: {}",
						newObject.getKey());
		}
	}
	DataVec::change(pos, newObject);
}

void TemplateVec::push_back(const Template &value) {
	TaskScheduler &TS = TaskScheduler::getInstance();
	if (!TS.getIsInitialized()) {
		SPDLOG_ERROR("Task scheduler isn't initialized");
		return;
	}

	// TODO: optimization: maybe having a map beforehand would be faster
	for (const auto &temp : vec) {
		if (value.getName() == temp.getName()) {
			SPDLOG_WARN("Adding a template with a name that's already taken. "
						"Use change()");
			return;
		}
	}

	if (value.getActiveTS()) {
		if (!TS.add(value)) {
			SPDLOG_ERROR("Couldn't add new task to task scheduler. Key: {}",
						 value.getKey());
		}
	}
	DataVec::push_back(value);
}

// overriden init for templateVec
bool TemplateVec::readVec(fs::path &path) {
	if (!DataVec::readVec(path)) {
		return false;
	}

	TaskScheduler &TS = TaskScheduler::getInstance();
	if (TS.getIsInitialized()) {
		for (Template &temp : vec) {
			if (temp.getActiveTS() != TS.exists(temp.getKey())) {
				SPDLOG_WARN("Different info on TS active locally and in TS");
			}
		}
	}

	return true;
}
