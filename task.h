#ifndef TASK_H
#define TASK_H

#include "errand.h"
#include "schedule.h"

#include <taskschd.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class Task : public Errand {
    friend class BackupBuilder;
public:
    friend std::ostream & operator << (std::ostream &out, const Task &task);

    bool saveLocal() const;
    bool deleteLocal() const;

    std::vector<std::shared_ptr<Schedule> > getSchedules() const { return schedules; }


protected:
    Task(std::string& key,
         std::string& latestId,
         std::string& name,
         std::vector<std::string> dests,
         std::vector<fs::path> srcs,
        std::vector<std::shared_ptr<Schedule>> scheds)
        : Errand{key, latestId, name, dests, srcs}, schedules{scheds} {};

    std::vector<std::shared_ptr<Schedule>> schedules;

private:
    HRESULT saveTaskScheduler() const;
    HRESULT deleteTaskScheduler() const;
};

class COMGuard {
public:
    COMGuard();
    ~COMGuard();
    HRESULT initNewTask();

    HRESULT putScheduleTrigger(const std::shared_ptr<Schedule> schedule) const;

    ComPtr<ITaskService> getService() const { return pService; }
    ComPtr<ITaskFolder> getRootFolder() const { return pRootFolder; }
    ComPtr<ITaskDefinition> getTask() const { return pTask; }
    ComPtr<ITriggerCollection> getTriggerCollection() const { return pTriggerCollection; }
    ComPtr<IActionCollection> getActionCollection() const { return pActionCollection; }
    ComPtr<IExecAction> getExecAction() const { return pExecAction; }
    ComPtr<IAction> getAction() const { return pAction; }
    HRESULT getLastError() const { return lastError; }
private:
    ComPtr<ITaskService> pService;
    ComPtr<ITaskFolder> pRootFolder;
    ComPtr<ITaskDefinition> pTask;
    ComPtr<ITriggerCollection> pTriggerCollection;
    ComPtr<IActionCollection> pActionCollection;
    ComPtr<IExecAction> pExecAction;
    ComPtr<IAction> pAction;

    HRESULT lastError = S_OK;

    HRESULT createDailyTrigger(const DailySchedule& daily) const;
    HRESULT createWeeklyTrigger(const WeeklySchedule& weekly) const;
    HRESULT createMonthlyTrigger(const MonthlySchedule& monthly) const;
    HRESULT createOnceTrigger(const OnceSchedule& once) const;
};

#endif // TASK_H
