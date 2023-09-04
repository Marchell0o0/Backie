#ifndef METADATA_H
#define METADATA_H

#include <string>
#include <map>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

struct FileMetadata {
    std::time_t modificationTimestamp;
    uintmax_t fileSize;
    std::string fileHash;
    bool isDeleted;
};

std::map<fs::path, FileMetadata> loadMdMap(const fs::path& jsonDestination);

bool saveMdMap(const std::map<fs::path, FileMetadata>& metadata, const fs::path& jsonDestination);

FileMetadata MdFromFile(const fs::path& file);

std::optional<std::map<fs::path, FileMetadata>> loadCombinedMdMap(const fs::path& jsonDestination);

#endif // METADATA_H
