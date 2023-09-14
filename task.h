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

    friend std::ostream & operator << (std::ostream &out, const Task &task);
protected:
    Task(std::string& key,
         std::string& latestId,
         std::string& name,
         BackupType type,
         std::vector<std::string> dests,
         std::vector<fs::path> srcs,
        std::vector<std::shared_ptr<Schedule>> scheds)
        : Errand{key, latestId, name, type, dests, srcs}, schedules{scheds} {};

    std::vector<std::shared_ptr<Schedule>> schedules;

private:
    HRESULT saveTaskScheduler() const;
    HRESULT deleteTaskScheduler() const;
};




#endif // TASK_H
