#include <QCoreApplication>

#include "datavec.h"
#include "utils.h"

int main(int argc, char* argv[]) {
	QCoreApplication a(argc, argv);

	if (argc < 3) {
		return 1;
	}
	if (strstr(argv[0], "--backup") != 0) {
		return 1;
	}

	SPDLOG_INFO("Searching for AppData");
	auto localDataFolder = findAppDataFolder();
	if (!localDataFolder) {
		SPDLOG_ERROR("Couldn't read/create appdata");
		return 1;
	}

	// TODO: You could change an empty vector's element
	SPDLOG_INFO("Reading/creating tamplates.json");
	fs::path templatesPath = *localDataFolder / "templates.json";
	TemplateVec& templates = TemplateVec::getInstance();
	if (!templates.readVec(templatesPath)) {
		SPDLOG_ERROR("Couldn't read/create templates.json");
		return 1;
	}

	bool found = false;
	for (const auto& temp : templates) {
		if (temp.getKey() == argv[1]) {
			found = true;
			// executeBackup(temp);
		}
	}

	if (!found) {
		SPDLOG_ERROR("Couldn't find template with key: {}", argv[1]);
		return 1;
	}

	return a.exec();
}
