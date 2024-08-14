#include "pch.h"

#include "taskscheduler.h"
#include "utils.h"

bool checkAndLogHRESULT(HRESULT hr, const std::string& action) {
	if (FAILED(hr)) {
		SPDLOG_ERROR("{}, HRESULT = {}", action, hr);
		return false;
	}
	return true;
}

bool TaskScheduler::initialize() {
	HRESULT hr = CoInitialize(NULL);

	if (FAILED(hr)) {
		return false;
	}
	hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER,
						  IID_ITaskService, (void**)&pService);
	if (FAILED(hr)) {
		return false;
	}
	hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(),
						   _variant_t());
	if (FAILED(hr)) {
		return false;
	}
	hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
	if (FAILED(hr)) {
		return false;
	}
	isInitialized = true;
	return true;
}

TaskScheduler::~TaskScheduler() {
	pAction = nullptr;
	pExecAction = nullptr;
	pActionCollection = nullptr;
	pTriggerCollection = nullptr;
	// pTask = nullptr;
	pRootFolder = nullptr;
	pService = nullptr;
	CoUninitialize();
	isInitialized = false;
}

bool TaskScheduler::exists(const std::string& key) {
	std::wstring taskName = L"Backie " + strToWStr(key);

	ComPtr<IRegisteredTask> pTask;
	HRESULT hr = pRootFolder->GetTask(_bstr_t(taskName.c_str()), &pTask);

	if (FAILED(hr)) {
		return false;
	}

	TASK_STATE taskState;
	pTask->get_State(&taskState);
	if (taskState == TASK_STATE_DISABLED) {
		SPDLOG_WARN("Task exists but is disabled. Key: {}", key);
	}
	return true;
}

bool TaskScheduler::erase(const std::string& key) {
	std::wstring taskName = L"Backie " + strToWStr(key);

	HRESULT hr = pRootFolder->DeleteTask(_bstr_t(taskName.c_str()), 0);
	if (FAILED(hr)) {
		return false;
	}
	return true;
}
bool TaskScheduler::add(const Template& temp) {
	ComPtr<ITaskDefinition> pTask = initNewTask();
	if (!pTask) {
		return false;
	}

	fs::path exePath
		= std::filesystem::current_path() / "BackieWorker" / "BackieWorker.exe";
	std::wstring args = L"/C \"" + exePath.wstring() + L" --backup "
						+ strToWStr(temp.getKey());

	pExecAction->put_Path(bstr_t(L"cmd.exe"));
	pExecAction->put_Arguments(bstr_t(args.c_str()));
	HRESULT hr;
	for (const auto& schedule : temp.getSchedules()) {
		hr = putScheduleTrigger(schedule);
		if (FAILED(hr)) {
			return false;
		}
	}

	std::wstring taskName = L"Backie " + strToWStr(temp.getKey());

	ComPtr<IRegisteredTask> pRegisteredTask = NULL;
	hr = pRootFolder->RegisterTaskDefinition(
		bstr_t(taskName.c_str()), pTask.Get(), TASK_CREATE_OR_UPDATE,
		_variant_t(), _variant_t(), TASK_LOGON_INTERACTIVE_TOKEN,
		_variant_t(L""), &pRegisteredTask);
	if (FAILED(hr)) {
		return false;
	}
	return true;
}
bool TaskScheduler::change(const Template& temp) { // Fetch the existing task
	std::wstring taskName = L"Backie " + strToWStr(temp.getKey());

	ComPtr<IRegisteredTask> pRegisteredTask;
	HRESULT hr
		= pRootFolder->GetTask(_bstr_t(taskName.c_str()), &pRegisteredTask);
	if (FAILED(hr)) {
		SPDLOG_DEBUG("Task didn't exist trying to add new");
		return add(temp);
		// return false;
	}

	ComPtr<ITaskDefinition> pTask;
	hr = pRegisteredTask->get_Definition(&pTask);
	if (FAILED(hr)) {
		return false;
	}

	// Clear existing triggers
	// ComPtr<ITriggerCollection> pTriggerCollection;
	hr = pTask->get_Triggers(&pTriggerCollection);
	if (FAILED(hr)) {
		return false;
	}

	hr = pTriggerCollection->Clear();
	if (FAILED(hr)) {
		return false;
	}

	for (const auto& schedule : temp.getSchedules()) {
		hr = putScheduleTrigger(schedule);
		if (FAILED(hr)) {
			return false;
		}
	}

	// Save the updated task definition
	hr = pRootFolder->RegisterTaskDefinition(
		_bstr_t(taskName.c_str()), pTask.Get(), TASK_UPDATE, _variant_t(),
		_variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(L""),
		&pRegisteredTask);

	if (FAILED(hr)) {
		return false;
	}
	return true;
}

ComPtr<ITaskDefinition> TaskScheduler::initNewTask() {
	ComPtr<ITaskDefinition> pTask;
	HRESULT hr = pService->NewTask(0, &pTask);
	if (FAILED(hr))
		return nullptr;

	hr = pTask->get_Actions(&pActionCollection);
	if (FAILED(hr))
		return nullptr;

	hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
	if (FAILED(hr))
		return nullptr;

	hr = pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction);
	if (FAILED(hr))
		return nullptr;

	hr = pTask->get_Triggers(&pTriggerCollection);
	return pTask;
}

HRESULT
TaskScheduler::putScheduleTrigger(
	const std::shared_ptr<Schedule> schedule) const {
	HRESULT hr;
	switch (schedule->getRecurrence()) {
	case ScheduleRecurrence::DAILY:
		hr = createDailyTrigger(dynamic_cast<const DailySchedule&>(*schedule));
		break;
	case ScheduleRecurrence::WEEKLY:
		hr = createWeeklyTrigger(
			dynamic_cast<const WeeklySchedule&>(*schedule));
		break;
	case ScheduleRecurrence::MONTHLY:
		hr = createMonthlyTrigger(
			dynamic_cast<const MonthlySchedule&>(*schedule));
		break;
	case ScheduleRecurrence::ONCE:
		hr = createOnceTrigger(dynamic_cast<const OnceSchedule&>(*schedule));
		break;
	default:
		return 1;
	}

	return hr;
}

HRESULT TaskScheduler::createDailyTrigger(const DailySchedule& daily) const {
	ComPtr<ITrigger> pTrigger = NULL;
	std::wstring startTime = L"2023-08-01T" + std::to_wstring(daily.hour) + L":"
							 + std::to_wstring(daily.minute) + L":00";

	HRESULT hr = pTriggerCollection->Create(TASK_TRIGGER_DAILY, &pTrigger);
	if (FAILED(hr))
		return hr;

	hr = pTrigger->put_StartBoundary(bstr_t(startTime.c_str()));
	return hr;
}

HRESULT TaskScheduler::createWeeklyTrigger(const WeeklySchedule& weekly) const {
	ComPtr<ITrigger> pTrigger = NULL;
	std::wstring startTime = L"2023-08-01T" + std::to_wstring(weekly.hour)
							 + L":" + std::to_wstring(weekly.minute) + L":00";

	HRESULT hr = pTriggerCollection->Create(TASK_TRIGGER_WEEKLY, &pTrigger);
	if (FAILED(hr))
		return hr;

	IWeeklyTrigger* pWeeklyTrigger = NULL;
	hr = pTrigger->QueryInterface(IID_IWeeklyTrigger, (void**)&pWeeklyTrigger);
	if (FAILED(hr))
		return hr;

	hr = pWeeklyTrigger->put_DaysOfWeek(1 << (weekly.day - 1));
	pWeeklyTrigger->Release();
	if (FAILED(hr))
		return hr;

	hr = pTrigger->put_StartBoundary(bstr_t(startTime.c_str()));
	return hr;
}

HRESULT
TaskScheduler::createMonthlyTrigger(const MonthlySchedule& monthly) const {
	ComPtr<ITrigger> pTrigger = NULL;
	std::wstring startTime = L"2023-08-01T" + std::to_wstring(monthly.hour)
							 + L":" + std::to_wstring(monthly.minute) + L":00";

	HRESULT hr = pTriggerCollection->Create(TASK_TRIGGER_MONTHLY, &pTrigger);
	if (FAILED(hr))
		return hr;

	IMonthlyTrigger* pMonthlyTrigger = NULL;
	hr = pTrigger->QueryInterface(IID_IMonthlyTrigger,
								  (void**)&pMonthlyTrigger);
	if (FAILED(hr))
		return hr;

	if (monthly.day == -1) {
		hr = pMonthlyTrigger->put_RunOnLastDayOfMonth(true);
	} else {
		hr = pMonthlyTrigger->put_DaysOfMonth(1 << (monthly.day - 1));
	}
	pMonthlyTrigger->Release();
	if (FAILED(hr))
		return hr;

	hr = pTrigger->put_StartBoundary(bstr_t(startTime.c_str()));
	return hr;
}

HRESULT TaskScheduler::createOnceTrigger(const OnceSchedule& once) const {
	ComPtr<ITrigger> pTrigger = NULL;
	std::wstring startTime
		= std::to_wstring(once.year) + L"-" + std::to_wstring(once.month) + L"-"
		  + std::to_wstring(once.day) + L"T" + std::to_wstring(once.hour) + L":"
		  + std::to_wstring(once.minute) + L":00";

	HRESULT hr = pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);
	if (FAILED(hr))
		return hr;

	hr = pTrigger->put_StartBoundary(bstr_t(startTime.c_str()));
	return hr;
}
