#include "pch.h"

#include "errand.h"
// #include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
// #include <map>

#include "settings.h"
#include "metadata.h"

std::string Errand::getKey() const {
    return this->key;
}
std::vector<std::string> Errand::getDestinations() const {
    return this->destinations;
}
std::vector<fs::path> Errand::getSources() const {
    return this->sources;
}
std::string Errand::getName() const {
    return this->name;
}

static std::string formatNewName() {
	auto now = std::chrono::system_clock::now();
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);
	std::tm buf;
	localtime_s(&buf,
				&now_time); // TODO: Use "localtime_r" for non-Windows platforms
	std::ostringstream oss;
	oss << std::put_time(&buf, "[%d-%m-%Y %H.%M]");

	return oss.str();
}

static bool myCopy(const fs::path& source, const fs::path& dest) {
	fs::create_directories(dest.parent_path());

	try {
		fs::copy(source, dest);
	} catch (const fs::filesystem_error& e) {
		SPDLOG_ERROR("Couldn't copy with this error: {}", e.what());
		return false;
	}
	return true;
}

bool Errand::perform() {
	Settings& settings = Settings::getInstance();
	for (const auto& destinationKey : this->destinations) {
		auto dest = settings.getDest(destinationKey);
		if (!dest) {
			SPDLOG_WARN("One of the destinations doesn't exist");
			continue;
		} else if (!fs::exists(dest->destinationFolder)) {
			SPDLOG_WARN("One of the destination folders doesn't exist");
			continue;
		}

		fs::path backupFolder = dest->destinationFolder / this->name;
		fs::path localFolder = backupFolder / formatNewName();

		if (fs::exists(localFolder)) {
			SPDLOG_WARN("Backup folder with name: {}, already exists",
						localFolder.u8string());
			continue;
		} else {
			fs::create_directories(localFolder);
		}
		SPDLOG_INFO("Initializing metadata");
		Metadata mtd(backupFolder, localFolder);

		SPDLOG_INFO("checking if files have changed");
		// Backup changed files
		for (const auto& source : this->sources) {
			fs::path sourceBackup = localFolder / source.filename();
			fs::create_directory(sourceBackup);
			for (const auto& file : fs::recursive_directory_iterator(source)) {
				if (!file.is_regular_file()) {
					continue;
				}
				fs::path relativePath = fs::relative(file.path(), source);

				if (mtd.changedUpdate(relativePath, source)) {
					myCopy(source / relativePath, sourceBackup / relativePath);
				}
			}

			// TODO: new mtd could be the same as old
		}
		SPDLOG_INFO("Calling destructor");
	}
	return true;
}
