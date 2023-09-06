#ifndef TASK_H
#define TASK_H

#include "errand.h"
#include "schedule.h"

class Task : public Errand {
    friend class BackupBuilder;
public:
    bool saveLocal() const;
    bool deleteLocal() const;

    std::vector<std::shared_ptr<Schedule> > getSchedules() const;
    std::string getName() const;

    friend std::ostream & operator << (std::ostream &out, const Task &task);
protected:
    Task(std::string& key,
         std::string& name,
         BackupType type,
         std::vector<std::string> dests,
         std::vector<fs::path> srcs,
        std::vector<std::shared_ptr<Schedule>> scheds)
        : Errand{key, type, dests, srcs}, schedules{scheds}, name{name} {};

    std::vector<std::shared_ptr<Schedule>> schedules;
    std::string name;

private:
    HRESULT saveTaskScheduler(Schedule& schedule) const;
    HRESULT deleteTaskScheduler() const;
};




#endif // TASK_H
