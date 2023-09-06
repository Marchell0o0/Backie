#ifndef ERRAND_H
#define ERRAND_H

#include <filesystem>
#include <windows.h>

#include "utils.h"

class Errand {
    friend class BackupBuilder;
public:
    bool perform() const;

    std::string getKey() const;
    BackupType getCurrentType() const;
    std::vector<std::string> getDestinations() const;
    std::vector<fs::path> getSources() const;
    std::string getName() const;
protected:
    Errand(std::string& key,
           std::string& name,
           BackupType type,
           std::vector<std::string> dests,
           std::vector<fs::path> srcs)
        : key{key}, name{name}, currentType{type}, destinations{dests}, sources{srcs} {};

    std::string key;
    std::string name;
    BackupType currentType;
    std::vector<std::string> destinations;
    std::vector<fs::path> sources;

private:
    bool full() const;
    bool incremental() const;
};


#endif // ERRAND_H
