#ifndef METADATA_H
#define METADATA_H

#include "nlohmann/json.hpp"
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

class Metadata {
public:
    Metadata(){};

    //TODO: Revise how paths are recorded
    void initAllFiles(const std::vector<fs::path>& sources);
    void initChangedFiles(const std::vector<fs::path>& sources,
                          const fs::path& destination,
                          const std::string& name);
    void initJson(const std::string& Id, const std::string& parentId);

    bool changed(const fs::path& file);
    void addFile(const fs::path& file);
    bool save(const fs::path& jsonDestination);
    std::map<fs::path, FileMetadata> map;
private:
    std::map<fs::path, FileMetadata> combinedOldMap;
    nlohmann::json MdJson;

    FileMetadata MdFromFile(const fs::path& file);
    std::map<fs::path, FileMetadata> loadMdMap(const fs::path& jsonDestination);
    std::optional<std::map<fs::path, FileMetadata>> loadCombinedMdMap(const fs::path& jsonDestination, const std::string& name);
};


#endif // METADATA_H
