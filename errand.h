#ifndef ERRAND_H
#define ERRAND_H

#include <filesystem>
#include <windows.h>

namespace fs = std::filesystem;

class Errand {
    friend class BackupBuilder;
public:
    bool perform();


    std::string getKey() const;
    std::vector<std::string> getDestinations() const;
    std::vector<fs::path> getSources() const;
    std::string getName() const;
protected:
    Errand(std::string& key,
           std::string& parentId,
           std::string& name,
           std::vector<std::string> dests,
           std::vector<fs::path> srcs)
        : key{key}, parentId{parentId}, name{name}, destinations{dests}, sources{srcs} {};

	std::string key;
	std::string name;
	std::string parentId;
	std::string Id;
	std::vector<std::string> destinations;
	std::vector<fs::path> sources;
};


#endif // ERRAND_H
