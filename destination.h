#ifndef DESTINATION_H
#define DESTINATION_H

#include <filesystem>

struct Destination
{
    std::string name;
    std::filesystem::path destinationFolder;
};

#endif // DESTINATION_H
