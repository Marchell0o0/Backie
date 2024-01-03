#ifndef METADATA_H
#define METADATA_H

#include "nlohmann/json.hpp"
#include <string>
#include <map>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

class Metadata {
public:
	Metadata(const fs::path& backupFolder, const fs::path& localFolder);
	Metadata(const fs::path& backupFolder);
	~Metadata();

	fs::path backupFolder;
	fs::path localFolder;
	std::string id;
	nlohmann::json localData;
	nlohmann::json globalData;

	void setNextParent(const fs::path& folder);

	bool changedUpdate(const fs::path& file, const fs::path& source);

	std::vector<fs::path> getBackupsVec() const;
};

#endif // METADATA_H
