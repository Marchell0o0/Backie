#ifndef TASKSCHEDULER_H
#define TASKSCHEDULER_H

#include "schedule.h"
#include "template.h"

bool checkAndLogHRESULT(HRESULT hr, const std::string& action);

class TaskScheduler {
	friend class TemplateVec;

public:
	// singleton
	TaskScheduler(TaskScheduler& other) = delete;
	void operator=(const TaskScheduler&) = delete;
	static TaskScheduler& getInstance() {
		static TaskScheduler instance;
		return instance;
	}
	~TaskScheduler();
	bool initialize();

private:
	TaskScheduler() {}
	bool exists(const std::string& key);
	bool erase(const std::string& key);
	bool add(const Template& temp);
	bool change(const Template& temp);

	// Getter functions
	bool getIsInitialized() const { return isInitialized; }
	ComPtr<ITaskService> getService() const { return pService; }
	ComPtr<ITaskFolder> getRootFolder() const { return pRootFolder; }
	// ComPtr<ITaskDefinition> getTask() const { return pTask; }
	ComPtr<ITriggerCollection> getTriggerCollection() const {
		return pTriggerCollection;
	}
	ComPtr<IActionCollection> getActionCollection() const {
		return pActionCollection;
	}
	ComPtr<IExecAction> getExecAction() const { return pExecAction; }
	ComPtr<IAction> getAction() const { return pAction; }

	bool isInitialized = false;
	ComPtr<ITaskService> pService;
	ComPtr<ITaskFolder> pRootFolder;
	ComPtr<ITriggerCollection> pTriggerCollection;
	ComPtr<IActionCollection> pActionCollection;
	ComPtr<IExecAction> pExecAction;
	ComPtr<IAction> pAction;

	ComPtr<ITaskDefinition> initNewTask();
	HRESULT
	putScheduleTrigger(const std::shared_ptr<Schedule> schedule) const;
	HRESULT createDailyTrigger(const DailySchedule& daily) const;
	HRESULT createWeeklyTrigger(const WeeklySchedule& weekly) const;
	HRESULT createMonthlyTrigger(const MonthlySchedule& monthly) const;
	HRESULT createOnceTrigger(const OnceSchedule& once) const;
};

#endif // TASKSCHEDULER_H
