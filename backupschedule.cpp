#include "backupschedule.h"

//#include <stdexcept>
#include <string>
//#include <iostream>
#include <filesystem>
#include <optional>
#include <array>
#include <comdef.h>
#include <windows.h>
#include <taskschd.h>
#include <wrl/client.h>

#include "spdlog/spdlog.h"

#include "utils.h"

using Microsoft::WRL::ComPtr;

BackupSchedule::BackupSchedule(BackupType type, std::filesystem::path dir, ScheduleRecurrence recurrence, std::array<int, 5> scheduleData)
    : type(type), directory(dir), recurrence(recurrence), scheduleData(scheduleData) {
}

BackupType BackupSchedule::getType() const {
    return type;
}

const std::string BackupSchedule::getTypeStr() {
    switch(type){
    case BackupType::FULL:
        return "FULL";
    case BackupType::INCREMENTAL:
        return "INCREMENTAL";
    default:
        return "";
    }
}

std::filesystem::path BackupSchedule::getDirectory() const {
    return directory;
}

ScheduleRecurrence BackupSchedule::getRecurrence() const {
    return recurrence;
}

int BackupSchedule::getYear() const {
    if (recurrence != ScheduleRecurrence::ONCE) {
        // handle error
    }
    return scheduleData[0];
}

int BackupSchedule::getMonth() const {
    if (recurrence != ScheduleRecurrence::ONCE) {
        // handle error
    }
    return scheduleData[1];
}

int BackupSchedule::getDay() const {
    switch (recurrence) {
    case ScheduleRecurrence::ONCE:
        return scheduleData[2];
    case ScheduleRecurrence::MONTHLY:
        return scheduleData[0];
    default:
        // handle error
        return 1;
    }
}

int BackupSchedule::getHour() const {
    switch (recurrence) {
    case ScheduleRecurrence::ONCE:
        return scheduleData[3];
    case ScheduleRecurrence::MONTHLY:
    case ScheduleRecurrence::WEEKLY:
        return scheduleData[1];
    case ScheduleRecurrence::DAILY:
        return scheduleData[0];
    default:
        // handle error
        return 1;
    }
}

int BackupSchedule::getMinute() const {
    switch (recurrence) {
    case ScheduleRecurrence::ONCE:
        return scheduleData[4];
    case ScheduleRecurrence::MONTHLY:
    case ScheduleRecurrence::WEEKLY:
        return scheduleData[2];
    case ScheduleRecurrence::DAILY:
        return scheduleData[1];
    default:
        // handle error
        return 1;
    }
}


std::wstring BackupSchedule::getTaskName() {
    std::wstring taskName;

    std::wstring recurrenceString;
    switch (recurrence) {
    case ScheduleRecurrence::ONCE:
        recurrenceString = L"singular";
        break;
    case ScheduleRecurrence::MONTHLY:
        recurrenceString = L"monthly";
        break;
    case ScheduleRecurrence::WEEKLY:
        recurrenceString = L"weekly";
        break;
    case ScheduleRecurrence::DAILY:
        recurrenceString = L"daily";
        break;
    }

    std::wstring typeString;
    switch (type) {
    case BackupType::FULL:
        typeString = L"Full";
        break;
    case BackupType::INCREMENTAL:
        typeString = L"Incremental";
        break;
    default:
        typeString = L"";
        break;
    }

    taskName = typeString + L" " + recurrenceString + L" backup of " + directory.wstring();

    // Cleanup for task scheduler rules
    std::replace(taskName.begin(), taskName.end(), '/', ' ');
    std::replace(taskName.begin(), taskName.end(), '\\', ' ');
    taskName.erase(std::remove(taskName.begin(), taskName.end(), ':'), taskName.end());

    return taskName;
}

std::wstring BackupSchedule::getTaskTime() {
    std::wstring startTime;
    if (recurrence == ScheduleRecurrence::ONCE) {
        startTime = std::to_wstring(getYear()) + L"-" + std::to_wstring(getMonth()) + L"-" + std::to_wstring(getDay()) + L"T" +
                    std::to_wstring(getHour()) + L":" + std::to_wstring(getMinute()) + L":00";
    } else {
        startTime = L"2023-08-01T" + std::to_wstring(getHour()) + L":" + std::to_wstring(getMinute()) + L":00";
    }
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
            //            SPDLOG_WARN("COM library was already initialized on this thread.");
        } else if (hr == RPC_E_CHANGED_MODE) {
            SPDLOG_ERROR("A previous call to CoInitializeEx specified a different threading model.");
        } else {
            SPDLOG_ERROR("Couldn't initialize COM library. Error code: {}", hr);
        }
    }
    return hr;
}

HRESULT BackupSchedule::addToTaskScheduler(){
    HRESULT hr = initializeCOM();

    if (FAILED(hr)) return hr;

    auto cleanup = [](auto &comObject)
    {
        if (comObject)
            comObject->Release();
    };

    ComPtr<ITaskService> pService = connectToTaskScheduler();
    if (!pService) return hr;

    ComPtr<ITaskFolder> pRootFolder;
    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr)) return hr;

    ComPtr<ITaskDefinition> pTask;
    hr = pService->NewTask(0, &pTask);
    if (FAILED(hr)) return hr;

    ComPtr<ITriggerCollection> pTriggerCollection;
    hr = pTask->get_Triggers(&pTriggerCollection);
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

    ITrigger* pTrigger = NULL;


    switch (recurrence)
    {
    case ScheduleRecurrence::DAILY:
        pTriggerCollection->Create(TASK_TRIGGER_DAILY, &pTrigger);

        break;

    case ScheduleRecurrence::WEEKLY: {
        pTriggerCollection->Create(TASK_TRIGGER_WEEKLY, &pTrigger);
        IWeeklyTrigger* pWeeklyTrigger = NULL;
        pTrigger->QueryInterface(IID_IWeeklyTrigger, (void**)&pWeeklyTrigger);
        pWeeklyTrigger->put_DaysOfWeek(1 << (getDay() - 1));
        pWeeklyTrigger->Release();

        break;
    }
    case ScheduleRecurrence::MONTHLY: {
        pTriggerCollection->Create(TASK_TRIGGER_MONTHLY, &pTrigger);
        IMonthlyTrigger* pMonthlyTrigger = NULL;
        pTrigger->QueryInterface(IID_IMonthlyTrigger, (void**)&pMonthlyTrigger);
        if (getDay() > 0) {
            pMonthlyTrigger->put_DaysOfMonth(1 << (getDay() - 1));
        } else {
            pMonthlyTrigger->put_RunOnLastDayOfMonth(true);
        }
        pMonthlyTrigger->Release();

        break;
    }
    case ScheduleRecurrence::ONCE:
    default:
        pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);

        break;
    }

    std::wstring taskName = getTaskName();
    std::wstring startTime = getTaskTime();

    std::filesystem::path exePath = std::filesystem::current_path() / "Backie.exe";
    std::wstring directory = getDirectory().wstring();
    std::wstring type = stringToWString(getTypeStr());

    std::wstring args = L"/C \"" + exePath.wstring() +
                        L" --backup " + directory + L" " + type +
                        L"\" >W:/Programming/Backie/backie_logs/log.txt 2>&1";


    pTrigger->put_StartBoundary(bstr_t(startTime.c_str()));

    pExecAction->put_Path(bstr_t(L"cmd.exe"));
    pExecAction->put_Arguments(bstr_t(args.c_str()));

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

    CoUninitialize();
    return hr;
}

bool BackupSchedule::deleteTask() {
    if(!initializeCOM()) return false;

    auto cleanup = [](auto &comObject)
    {
        if (comObject)
            comObject->Release();
    };

    ComPtr<ITaskService> pService = connectToTaskScheduler();
    if (!pService) return false;

    ComPtr<ITaskFolder> pRootFolder;
    HRESULT hr = pService->GetFolder(bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr)) return false;

    std::wstring taskName = getTaskName();

    hr = pRootFolder->DeleteTask(bstr_t(taskName.c_str()), 0);
    if (FAILED(hr)) return false;

    CoUninitialize();
    return true;
}

