#include "pch.h"

#include "metadata.h"

// #include <fstream>
// #include <optional>
#include <map>
// #include <windows.h>

// #include "nlohmann/json.hpp"
// #include "spdlog/spdlog.h"

namespace fs = std::filesystem;

static std::string getCurrentTimestamp() {
	auto now = std::chrono::system_clock::now();

	auto now_time_t = std::chrono::system_clock::to_time_t(now);

	std::tm now_tm = *std::gmtime(&now_time_t);

	std::ostringstream stream;

	stream << std::put_time(&now_tm, "%Y-%m-%dT%H:%M:%SZ"); // ISO 8601 format

	return stream.str();
}

Metadata::Metadata(const fs::path& backupFolder) : backupFolder{backupFolder} {
	fs::path globalMtdPath = backupFolder / "global_metadata.json";
	if (!fs::exists(globalMtdPath)) {
		globalData["latest folder"] = "";
		std::ofstream globalMtd(globalMtdPath);
		globalMtd << globalData.dump(4);
	} else {
		std::ifstream globalMtd(globalMtdPath);
		globalMtd >> globalData;
	}
}

Metadata::Metadata(const fs::path& backupFolder, const fs::path& localFolder)
	: backupFolder{backupFolder}, localFolder{localFolder} {

	fs::path localMtdPath = localFolder / "local_metadata.json";
	fs::path globalMtdPath = backupFolder / "global_metadata.json";
	fs::path lastBackup;

	if (!fs::exists(globalMtdPath)) {
		//		globalData["latest folder"] = "";
		//		std::ofstream globalMtd(globalMtdPath);
		//		globalMtd << globalData.dump(4);
	} else {
		std::ifstream globalMtd(globalMtdPath);
		globalMtd >> globalData;
		fs::path lastFolder = globalData["latest folder"];
		lastBackup = lastFolder / "local_metadata.json";
	}

	SPDLOG_INFO("Initializing local data");

	//	if (fs::exists(localMtdPath)) {
	//		std::ifstream file(localMtdPath);
	//		file >> localData;
	//	}
	if (!lastBackup.empty()) {
		std::ifstream file(lastBackup);
		file >> localData;
	}
	//		else {
	//		localData["deleted"] = nlohmann::json::object();
	//		std::ofstream localMtd(localMtdPath);
	//		localMtd << localData.dump(4);
	//	}

	//	id = generate_uuid_v4();
}

Metadata::~Metadata() {

	if (globalData.find("latest folder") != globalData.end()) {
		std::string parentFolder = globalData["latest folder"];
		globalData[localFolder.string()]
			= {{"parent folder", parentFolder},
			   {"children folders", nlohmann::json::array()},
			   {"timestamp", getCurrentTimestamp()}};
		globalData[parentFolder]["children folders"].push_back(localFolder);
	} else {
		globalData[localFolder.string()]
			= {{"parent folder", ""},
			   {"children folders", nlohmann::json::array()},
			   {"timestamp", getCurrentTimestamp()}};
	}
	globalData["latest folder"] = localFolder.string();

	std::ofstream globalMtd(backupFolder / "global_metadata.json");
	if (!globalMtd) {
		SPDLOG_ERROR("Failed to open global metadata file for writing");
	}
	globalMtd << globalData.dump(4);
	globalMtd.close();

	// TODO: new local data is the same as the last
	if (localFolder.empty()) {
		return;
	}

	std::vector<std::string> emptyBackups;
	for (const auto& [backupFolder, sources] : localData.items()) {
		std::vector<std::string> emptySources;
		for (const auto& [source, filePaths] : sources.items()) {

			std::vector<std::string> deletedPaths;
			for (auto& [path, hash] : filePaths.items()) {
				fs::path object = path;
				fs::path sourcePath = source;
				fs::path objectAbsPath = sourcePath / object;
				if (!fs::exists(objectAbsPath)) {
					deletedPaths.push_back(path);
				}
			}

			for (const auto& path : deletedPaths) {
				filePaths.erase(path);
			}

			if (filePaths.empty()) {
				emptySources.push_back(source);
			}
		}
		for (const auto& emptySource : emptySources) {
			sources.erase(emptySource);
		}
		if (sources.empty()) {
			emptyBackups.push_back(backupFolder);
		}
	}
	for (const auto& emptyBackup : emptyBackups) {
		localData.erase(emptyBackup);
	}

	std::ofstream localMtd(localFolder / "local_metadata.json");
	if (!localMtd) {
		SPDLOG_ERROR("Failed to open local metadata file for writing");
	}

	localMtd << localData.dump(4);
	localMtd.close();

	// TODO: Add other attributes
	//	SetFileAttributes(destination.c_str(),
	//					  GetFileAttributes(destination.c_str())
	//						  | FILE_ATTRIBUTE_HIDDEN);
}

std::vector<fs::path> Metadata::getBackupsVec() const {
	std::vector<fs::path> backups;
	for (auto& [folder, values] : globalData.items()) {
		if (folder != "latest folder" && !folder.empty()
			&& fs::exists(folder)) {
			backups.push_back(folder);
		}
	}
	return backups;
}

void Metadata::setNextParent(const fs::path& folder) {
	globalData["latest folder"] = folder.string();
}

bool Metadata::changedUpdate(const fs::path& relativePath,
							 const fs::path& source) {
	std::string newHash
		= std::to_string(fs::last_write_time(source / relativePath)
							 .time_since_epoch()
							 .count()); // TODO: hash

	for (auto& [backupFolder, sources] : localData.items()) {
		if (sources.find(source.string()) == sources.end()) {
			continue;
		}
		for (auto& filePaths : sources.items()) {
			auto file = filePaths.value().find(relativePath.string());
			if (file == filePaths.value().end()) {
				continue;
			}

			const std::string& oldHash = file.value();

			if (oldHash != newHash) {
				localData[backupFolder][source.string()].erase(
					relativePath.string());
				localData[localFolder.string()][source.string()]
						 [relativePath.string()]
					= newHash;
				return true;
			} else {
				return false;
			}
		}
	}
	localData[localFolder.string()][source.string()][relativePath.string()]
		= newHash; // TODO: hash
	return true;
}
