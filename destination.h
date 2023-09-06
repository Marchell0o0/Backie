#ifndef DESTINATION_H
#define DESTINATION_H

#include <filesystem>
#include "utils.h"

class Destination
{
public:
    //TODO: This construcor shouldn't be here
    Destination() {};
    Destination(std::string n, std::filesystem::path destFldr)
        : name{n}, destinationFolder{destFldr}, key{generate_uuid_v4()} {}
    Destination(std::string n, std::filesystem::path destFldr, std::string k)
        : name{n}, destinationFolder{destFldr}, key{k} {}

    std::string getKey() const { return key; }

    std::string name;
    std::filesystem::path destinationFolder;
    friend std::ostream & operator << (std::ostream &out, const Destination &dest);

private:
    std::string key;
};

#endif // DESTINATION_H
