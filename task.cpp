#include "task.h"


#include <string>
#include <filesystem>
#include <comdef.h>
#include <windows.h>
#include <taskschd.h>
#include <wrl/client.h>

#include "spdlog/spdlog.h"

#include "utils.h"
#include "errand.h"
#include "settings.h"

using Microsoft::WRL::ComPtr;

std::vector<std::shared_ptr<Schedule>> Task::getSchedules() const {
    return this->schedules;
}

std::ostream & operator << (std::ostream &out, const Task &task) {
    out << "Name: " << task.name << ", current type: " << strFromType(task.getCurrentType()) << ", key: " << task.key << std::endl;
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
    out << "Schedule types: ";
    for (auto& schedule : task.schedules) {
        out << strFromType(schedule->type) << " ";
    }
    out << std::endl;
    return out;
}

//TODO: Just deleting everything and adding it again isn't the best solution
bool Task::saveLocal() const {
    if (FAILED(this->deleteTaskScheduler())) {
        return false;
    }
//    for (auto& schedule : this->schedules){
    if (FAILED(this->saveTaskScheduler())) {
        return false;
    }
//    }

    Settings& settings = Settings::getInstance();

    return settings.addUpdate(*this);
}

bool Task::deleteLocal() const {
    if (FAILED(this->deleteTaskScheduler())) {
        return false;
    }

    Settings& settings = Settings::getInstance();
    return settings.remove(*this);
}

//static std::wstring getTaskName(BackupType type, ScheduleRecurrence recurrence, const std::string& key) {
//    std::wstring recurrenceString;
//    switch (recurrence) {
//    case ScheduleRecurrence::ONCE:
//        recurrenceString = L"singular";
//        break;
//    case ScheduleRecurrence::MONTHLY:
//        recurrenceString = L"monthly";
//        break;
//    case ScheduleRecurrence::WEEKLY:
//        recurrenceString = L"weekly";
//        break;
//    case ScheduleRecurrence::DAILY:
//        recurrenceString = L"daily";
//        break;
//    }

//    std::wstring typeString = ;

//    //TODO: Maybe new name shouldn't be crated
//    std::wstring taskName;
////    taskName = typeString + L" " + recurrenceString + L" " + strToWStr(key);
//    taskName = strToWStr(key);

//    // Cleanup for task scheduler rules
//    std::replace(taskName.begin(), taskName.end(), '/', ' ');
//    std::replace(taskName.begin(), taskName.end(), '\\', ' ');
//    taskName.erase(std::remove(taskName.begin(), taskName.end(), ':'), taskName.end());

//    return taskName;
//}

static std::wstring getTaskTime(int year, int month, int day, int hour, int minute) {
    std::wstring startTime;

    startTime = std::to_wstring(year) + L"-" + std::to_wstring(month) + L"-" + std::to_wstring(day) + L"T" +
                std::to_wstring(hour) + L":" + std::to_wstring(minute) + L":00";
    return startTime;
}

static std::wstring getTaskTime(int hour, int minute){
    std::wstring startTime;
    startTime = L"2023-08-01T" + std::to_wstring(hour) + L":" + std::to_wstring(minute) + L":00";
    return startTime;
}

static ITaskService* connectToTaskScheduler(){
    ITaskService* pService = NULL;
    HRESULT hr = CoCreateInstance(CLSID_TaskScheduler,
                                  NULL,
                                  CLSCTX_INPROC_SERVER,
                                  IID_ITaskService,
                                  (void**)&pService);

    if (FAILED(hr))
        return NULL;

    hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
    if (FAILED(hr))
    {
        pService->Release();
        return NULL;
    }

    return pService;
}

static HRESULT initializeCOM(){
    HRESULT hr = CoInitialize(NULL);
    if (hr != S_OK)
    {
        if (hr == S_FALSE) {
//          SPDLOG_WARN("COM library was already initialized on this thread.");
        } else if (hr == RPC_E_CHANGED_MODE) {
            SPDLOG_ERROR("A previous call to CoInitializeEx specified a different threading model.");
        } else {
            SPDLOG_ERROR("Couldn't initialize COM library. Error code: {}", hr);
        }
    }
    return hr;
}

HRESULT Task::saveTaskScheduler() const {
    HRESULT hr = initializeCOM();

    if (FAILED(hr)) return hr;

    ComPtr<ITaskService> pService = connectToTaskScheduler();
    if (!pService) return hr;

    ComPtr<ITaskFolder> pRootFolder;
    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr)) return hr;


    // Group schedules by their type
    std::map<BackupType, std::vector<std::shared_ptr<Schedule>>> groupedSchedules;
    for (const auto& schedule : schedules) {
        groupedSchedules[schedule->type].push_back(schedule);
    }

    for (const auto& [type, schedulesOfType] : groupedSchedules) {

        ComPtr<ITaskDefinition> pTask;
        hr = pService->NewTask(0, &pTask);
        if (FAILED(hr)) return hr;


        ComPtr<IActionCollection> pActionCollection;
        hr = pTask->get_Actions(&pActionCollection);
        if (FAILED(hr)) return hr;

        ComPtr<IAction> pAction;
        hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
        if (FAILED(hr)) return hr;

        ComPtr<IExecAction> pExecAction;
        hr = pAction->QueryInterface(IID_IExecAction, (void **)&pExecAction);
        if (FAILED(hr)) return hr;

        std::filesystem::path exePath = std::filesystem::current_path() / "Backie.exe";
        std::wstring keyWStr = strToWStr(this->key);
        std::wstring typeWStr = strToWStr(strFromType(type));

        std::wstring args = L"/C \"" + exePath.wstring() +
                            L" --backup " + keyWStr + L" " + typeWStr;

        pExecAction->put_Path(bstr_t(L"cmd.exe"));
        pExecAction->put_Arguments(bstr_t(args.c_str()));

        for (const auto& schedule : schedulesOfType) {
            ITrigger* pTrigger = NULL;

            ComPtr<ITriggerCollection> pTriggerCollection;
            hr = pTask->get_Triggers(&pTriggerCollection);
            if (FAILED(hr)) return hr;

            std::wstring startTime;
            switch (schedule->getRecurrence())
            {
            case ScheduleRecurrence::DAILY: {
                const DailySchedule& daily = dynamic_cast<const DailySchedule&>(*schedule);
                startTime = getTaskTime(daily.hour, daily.minute);

                pTriggerCollection->Create(TASK_TRIGGER_DAILY, &pTrigger);
                break;
            }
            case ScheduleRecurrence::WEEKLY: {
                const WeeklySchedule& weekly = dynamic_cast<const WeeklySchedule&>(*schedule);
                startTime = getTaskTime(weekly.hour, weekly.minute);

                pTriggerCollection->Create(TASK_TRIGGER_WEEKLY, &pTrigger);
                IWeeklyTrigger* pWeeklyTrigger = NULL;
                pTrigger->QueryInterface(IID_IWeeklyTrigger, (void**)&pWeeklyTrigger);
                pWeeklyTrigger->put_DaysOfWeek(1 << (weekly.day - 1));
                pWeeklyTrigger->Release();

                break;
            }
            case ScheduleRecurrence::MONTHLY: {
                const MonthlySchedule& monthly = dynamic_cast<const MonthlySchedule&>(*schedule);
                startTime = getTaskTime(monthly.hour, monthly.minute);

                pTriggerCollection->Create(TASK_TRIGGER_MONTHLY, &pTrigger);
                IMonthlyTrigger* pMonthlyTrigger = NULL;
                pTrigger->QueryInterface(IID_IMonthlyTrigger, (void**)&pMonthlyTrigger);
                if (monthly.day > 0) {
                    pMonthlyTrigger->put_DaysOfMonth(1 << (monthly.day - 1));
                } else {
                    pMonthlyTrigger->put_RunOnLastDayOfMonth(true);
                }
                pMonthlyTrigger->Release();

                break;
            }
            case ScheduleRecurrence::ONCE: {
                const OnceSchedule& once = dynamic_cast<const OnceSchedule&>(*schedule);
                startTime = getTaskTime(once.year, once.month, once.day, once.hour, once.minute);

                pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);
                break;
            }
            default:
                break;
            }
            pTrigger->put_StartBoundary(bstr_t(startTime.c_str()));
        }

        std::wstring taskName = L"Backie " + strToWStr(strFromType(type)) + L" " + strToWStr(this->key);
        IRegisteredTask* pRegisteredTask = NULL;
        if (FAILED(pRootFolder->RegisterTaskDefinition(bstr_t(taskName.c_str()),
                                                       pTask.Get(),
                                                       TASK_CREATE_OR_UPDATE,
                                                       _variant_t(),
                                                       _variant_t(),
                                                       TASK_LOGON_INTERACTIVE_TOKEN,
                                                       _variant_t(L""),
                                                       &pRegisteredTask)))
            return hr;
    }

    CoUninitialize();
    return hr;
}

HRESULT Task::deleteTaskScheduler() const {
    HRESULT hr = initializeCOM();

    ComPtr<ITaskService> pService = connectToTaskScheduler();
    if (!pService) return hr;

    ComPtr<ITaskFolder> pRootFolder;
    hr = pService->GetFolder(bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr)) return hr;


    ComPtr<IRegisteredTaskCollection> pTaskCollection;
    hr = pRootFolder->GetTasks(0, &pTaskCollection);
    if (FAILED(hr)) return hr;

    LONG numTasks = 0;
    hr = pTaskCollection->get_Count(&numTasks);
    if (FAILED(hr)) return hr;

    bool deleteSuccess = true;

    for (LONG i = 1; i <= numTasks; ++i) { // COM collections are usually 1-based
        ComPtr<IRegisteredTask> pRegisteredTask;
        hr = pTaskCollection->get_Item(_variant_t(i), &pRegisteredTask);
        if (SUCCEEDED(hr)) {
            BSTR taskNameBSTR;
            hr = pRegisteredTask->get_Name(&taskNameBSTR);
            if (SUCCEEDED(hr)) {
                std::wstring taskName(taskNameBSTR, SysStringLen(taskNameBSTR));
                SysFreeString(taskNameBSTR); // Free the BSTR

                if (taskName.find(strToWStr(this->key)) != std::wstring::npos) { // If the task name contains this->key
                    hr = pRootFolder->DeleteTask(_bstr_t(taskName.c_str()), 0);
                    if (FAILED(hr)) {
                        deleteSuccess = false; // Failed to delete at least one task
                    }
                }
            }
        }
    }

    CoUninitialize();
    return deleteSuccess;
}

