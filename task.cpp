#include "task.h"

#include <comdef.h>
//#include <windows.h>
//#include <taskschd.h>
//#include <wrl/client.h>

#include "spdlog/spdlog.h"

#include "utils.h"
#include "errand.h"
#include "settings.h"

std::ostream & operator << (std::ostream &out, const Task &task) {
    out << "Name: " << task.name << ", key: " << task.key << std::endl;
    Settings& settings = Settings::getInstance();
    out << "Destinations:" << std::endl;
    for (auto& destinationKey : task.destinations) {
        auto dest = settings.getDest(destinationKey);
        if (dest) {
            out << *dest << std::endl;
        }
    }
    out << "Sources: ";
    for (auto& source : task.sources) {
        out << source.string() << std::endl;
    }
    out << std::endl;
    return out;
}

//TODO: Test if works as expected and updates the task(probably not)
bool Task::saveLocal() const {
    if (FAILED(this->saveTaskScheduler())) {
        return false;
    }
    Settings& settings = Settings::getInstance();

    return settings.addUpdate(*this);
}

bool Task::deleteLocal() const {
    this->deleteTaskScheduler();

    Settings& settings = Settings::getInstance();
    return settings.remove(*this);
}

static bool checkAndLog(HRESULT hr, const std::string& action) {
    if (FAILED(hr)) {
        SPDLOG_ERROR("{}, HRESULT = {}", action, hr);
        return false;
    }
    return true;
}

HRESULT Task::saveTaskScheduler() const {
    COMGuard guard;
    HRESULT hr = guard.getLastError();
    if (!checkAndLog(hr, "Error initializing COMGuard")) return hr;

    hr = guard.initNewTask();
    if (!checkAndLog(hr, "Error initializing new task")) return hr;

    fs::path exePath = std::filesystem::current_path() / "Backie.exe";
    std::wstring args = L"/C \"" + exePath.wstring() +
                        L" --backup " + strToWStr(this->key);

    guard.getExecAction()->put_Path(bstr_t(L"cmd.exe"));
    guard.getExecAction()->put_Arguments(bstr_t(args.c_str()));

    for (const auto& schedule : this->schedules) {
        hr = guard.putScheduleTrigger(schedule);
        if (!checkAndLog(hr, "Error creating schedule trigger")) return hr;
    }

    std::wstring taskName = L"Backie " + strToWStr(this->key);

    ComPtr<IRegisteredTask> pRegisteredTask = NULL;
    hr = guard.getRootFolder()->RegisterTaskDefinition(bstr_t(taskName.c_str()),
                                                       guard.getTask().Get(),
                                                       TASK_CREATE_OR_UPDATE,
                                                       _variant_t(),
                                                       _variant_t(),
                                                       TASK_LOGON_INTERACTIVE_TOKEN,
                                                       _variant_t(L""),
                                                       &pRegisteredTask);
    checkAndLog(hr, "Error registering new task");
    return hr;
}

HRESULT Task::deleteTaskScheduler() const {
    COMGuard guard;
    HRESULT hr = guard.getLastError();
    if (!checkAndLog(hr, "Error initializing COMGuard")) return hr;


    std::wstring taskName = L"Backie " + strToWStr(this->key);
    hr = guard.getRootFolder()->DeleteTask(_bstr_t(taskName.c_str()), 0);
    checkAndLog(hr, "Error deleting task");
    return hr;
}

COMGuard::COMGuard() {
    HRESULT hr = CoInitialize(NULL);

    if (FAILED(hr)) {
        lastError = hr;
        return;
    }
    hr = CoCreateInstance(CLSID_TaskScheduler,
                                  NULL,
                                  CLSCTX_INPROC_SERVER,
                                  IID_ITaskService,
                                  (void**)&pService);
    if (FAILED(hr)) {
        lastError = hr;
        return;
    }
    hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
    if (FAILED(hr)) {
        lastError = hr;
        return;
    }
    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr)) {
        lastError = hr;
        return;
    }
}

HRESULT COMGuard::initNewTask() {
    HRESULT hr = pService->NewTask(0, &pTask);
    if (FAILED(hr)) return hr;

    hr = pTask->get_Actions(&pActionCollection);
    if (FAILED(hr)) return hr;

    hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
    if (FAILED(hr)) return hr;

    hr = pAction->QueryInterface(IID_IExecAction, (void **)&pExecAction);
    if (FAILED(hr)) return hr;

    hr = pTask->get_Triggers(&pTriggerCollection);
    return hr;
}

COMGuard::~COMGuard() {
    CoUninitialize();
}

HRESULT COMGuard::putScheduleTrigger(const std::shared_ptr<Schedule> schedule) const {
    HRESULT hr;
    switch (schedule->getRecurrence())
    {
    case ScheduleRecurrence::DAILY:
        hr = createDailyTrigger(dynamic_cast<const DailySchedule&>(*schedule));
        break;
    case ScheduleRecurrence::WEEKLY:
        hr = createWeeklyTrigger(dynamic_cast<const WeeklySchedule&>(*schedule));
        break;
    case ScheduleRecurrence::MONTHLY:
        hr = createMonthlyTrigger(dynamic_cast<const MonthlySchedule&>(*schedule));
        break;
    case ScheduleRecurrence::ONCE:
        hr = createOnceTrigger(dynamic_cast<const OnceSchedule&>(*schedule));
        break;
    }
    return hr;
}

HRESULT COMGuard::createDailyTrigger(const DailySchedule& daily) const {
    ComPtr<ITrigger> pTrigger = NULL;
    std::wstring startTime = L"2023-08-01T"
                             + std::to_wstring(daily.hour) + L":"
                             + std::to_wstring(daily.minute) + L":00";

    HRESULT hr = pTriggerCollection->Create(TASK_TRIGGER_DAILY, &pTrigger);
    if (FAILED(hr)) return hr;

    hr = pTrigger->put_StartBoundary(bstr_t(startTime.c_str()));
    return hr;
}

HRESULT COMGuard::createWeeklyTrigger(const WeeklySchedule& weekly) const {
    ComPtr<ITrigger> pTrigger = NULL;
    std::wstring startTime = L"2023-08-01T"
                             + std::to_wstring(weekly.hour) + L":"
                             + std::to_wstring(weekly.minute) + L":00";

    HRESULT hr = pTriggerCollection->Create(TASK_TRIGGER_WEEKLY, &pTrigger);
    if (FAILED(hr)) return hr;

    IWeeklyTrigger* pWeeklyTrigger = NULL;
    hr = pTrigger->QueryInterface(IID_IWeeklyTrigger, (void**)&pWeeklyTrigger);
    if (FAILED(hr)) return hr;

    hr = pWeeklyTrigger->put_DaysOfWeek(1 << (weekly.day - 1));
    pWeeklyTrigger->Release();
    if (FAILED(hr)) return hr;

    hr = pTrigger->put_StartBoundary(bstr_t(startTime.c_str()));
    return hr;
}

HRESULT COMGuard::createMonthlyTrigger(const MonthlySchedule& monthly) const {
    ComPtr<ITrigger> pTrigger = NULL;
    std::wstring startTime = L"2023-08-01T"
                             + std::to_wstring(monthly.hour) + L":"
                             + std::to_wstring(monthly.minute) + L":00";

    HRESULT hr = pTriggerCollection->Create(TASK_TRIGGER_MONTHLY, &pTrigger);
    if (FAILED(hr)) return hr;

    IMonthlyTrigger* pMonthlyTrigger = NULL;
    hr = pTrigger->QueryInterface(IID_IMonthlyTrigger, (void**)&pMonthlyTrigger);
    if (FAILED(hr)) return hr;

    if (monthly.day == -1) {
        hr = pMonthlyTrigger->put_RunOnLastDayOfMonth(true);
    } else {
        hr = pMonthlyTrigger->put_DaysOfMonth(1 << (monthly.day - 1));
    }
    pMonthlyTrigger->Release();
    if (FAILED(hr)) return hr;

    hr = pTrigger->put_StartBoundary(bstr_t(startTime.c_str()));
    return hr;
}

HRESULT COMGuard::createOnceTrigger(const OnceSchedule& once) const {
    ComPtr<ITrigger> pTrigger = NULL;
    std::wstring startTime = std::to_wstring(once.year) + L"-"
                             + std::to_wstring(once.month) + L"-"
                             + std::to_wstring(once.day) + L"T"
                             + std::to_wstring(once.hour) + L":"
                             + std::to_wstring(once.minute) + L":00";

    HRESULT hr = pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);
    if (FAILED(hr)) return hr;

    hr = pTrigger->put_StartBoundary(bstr_t(startTime.c_str()));
    return hr;
}


