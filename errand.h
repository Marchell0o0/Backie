#ifndef ERRAND_H
#define ERRAND_H

#include <filesystem>
#include <windows.h>

#include "utils.h"

class Errand {
    friend class BackupBuilder;
public:
    bool perform();

    void setCurrentType(BackupType type);

    std::string getKey() const;
    BackupType getCurrentType() const;
    std::vector<std::string> getDestinations() const;
    std::vector<fs::path> getSources() const;
    std::string getName() const;
//    std::string getParentId() const;
protected:
    Errand(std::string& key,
           std::string& parentId,
           std::string& name,
           BackupType type,
           std::vector<std::string> dests,
           std::vector<fs::path> srcs)
        : key{key}, parentId{parentId}, name{name}, currentType{type}, destinations{dests}, sources{srcs} {};

    std::string key;
    std::string name;
    std::string parentId;
    std::string Id;
    BackupType currentType;
    std::vector<std::string> destinations;
    std::vector<fs::path> sources;


private:
    bool full();
    bool incremental();
};


#endif // ERRAND_H
