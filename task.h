#ifndef TASK_H
#define TASK_H

#include <string>

struct Task {
    std::string directory;
    std::string type;
    std::string time;
    std::string filter;
};

#endif // TASK_H
