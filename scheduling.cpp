#include <string>
#include <filesystem>
#include <comdef.h>
#include <windows.h>
#include <taskschd.h>
#include <wrl/client.h>

#include "spdlog/spdlog.h"

#include "utils.h"
#include "backuperrand.h"

using Microsoft::WRL::ComPtr;

//std::wstring Backup::getTaskName() {
//    std::wstring recurrenceString;
//    switch (this->recurrence) {
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

//    std::wstring typeString = strToWStr(strFromType(this->type));

//    std::wstring taskName;
//    taskName = typeString + L" " + recurrenceString + L" " + strToWStr(this->name);

//    // Cleanup for task scheduler rules
//    std::replace(taskName.begin(), taskName.end(), '/', ' ');
//    std::replace(taskName.begin(), taskName.end(), '\\', ' ');
//    taskName.erase(std::remove(taskName.begin(), taskName.end(), ':'), taskName.end());

//    return taskName;
//}

//std::wstring Backup::getTaskTime() {
//    std::wstring startTime;
//    if (recurrence == ScheduleRecurrence::ONCE) {
//        startTime = std::to_wstring(this->year) + L"-" + std::to_wstring(this->month) + L"-" + std::to_wstring(this->day) + L"T" +
//                    std::to_wstring(this->hour) + L":" + std::to_wstring(this->minute) + L":00";
//    } else {
//        startTime = L"2023-08-01T" + std::to_wstring(this->hour) + L":" + std::to_wstring(this->minute) + L":00";
//    }
//    return startTime;
//}

//static ITaskService* connectToTaskScheduler(){
//    ITaskService* pService = NULL;
//    HRESULT hr = CoCreateInstance(CLSID_TaskScheduler,
//                                  NULL,
//                                  CLSCTX_INPROC_SERVER,
//                                  IID_ITaskService,
//                                  (void**)&pService);

//    if (FAILED(hr))
//        return NULL;

//    hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
//    if (FAILED(hr))
//    {
//        pService->Release();
//        return NULL;
//    }

//    return pService;
//}

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

//HRESULT Backup::addToTaskScheduler(){
//    HRESULT hr = initializeCOM();

//    if (FAILED(hr)) return hr;

//    auto cleanup = [](auto &comObject)
//    {
//        if (comObject)
//            comObject->Release();
//    };

//    ComPtr<ITaskService> pService = connectToTaskScheduler();
//    if (!pService) return hr;

//    ComPtr<ITaskFolder> pRootFolder;
//    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
//    if (FAILED(hr)) return hr;

//    ComPtr<ITaskDefinition> pTask;
//    hr = pService->NewTask(0, &pTask);
//    if (FAILED(hr)) return hr;

//    ComPtr<ITriggerCollection> pTriggerCollection;
//    hr = pTask->get_Triggers(&pTriggerCollection);
//    if (FAILED(hr)) return hr;

//    ComPtr<IActionCollection> pActionCollection;
//    hr = pTask->get_Actions(&pActionCollection);
//    if (FAILED(hr)) return hr;

//    ComPtr<IAction> pAction;
//    hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
//    if (FAILED(hr)) return hr;

//    ComPtr<IExecAction> pExecAction;
//    hr = pAction->QueryInterface(IID_IExecAction, (void **)&pExecAction);
//    if (FAILED(hr)) return hr;

//    ITrigger* pTrigger = NULL;


//    switch (recurrence)
//    {
//    case ScheduleRecurrence::DAILY:
//        pTriggerCollection->Create(TASK_TRIGGER_DAILY, &pTrigger);

//        break;

//    case ScheduleRecurrence::WEEKLY: {
//        pTriggerCollection->Create(TASK_TRIGGER_WEEKLY, &pTrigger);
//        IWeeklyTrigger* pWeeklyTrigger = NULL;
//        pTrigger->QueryInterface(IID_IWeeklyTrigger, (void**)&pWeeklyTrigger);
//        pWeeklyTrigger->put_DaysOfWeek(1 << (getDay() - 1));
//        pWeeklyTrigger->Release();

//        break;
//    }
//    case ScheduleRecurrence::MONTHLY: {
//        pTriggerCollection->Create(TASK_TRIGGER_MONTHLY, &pTrigger);
//        IMonthlyTrigger* pMonthlyTrigger = NULL;
//        pTrigger->QueryInterface(IID_IMonthlyTrigger, (void**)&pMonthlyTrigger);
//        if (getDay() > 0) {
//            pMonthlyTrigger->put_DaysOfMonth(1 << (getDay() - 1));
//        } else {
//            pMonthlyTrigger->put_RunOnLastDayOfMonth(true);
//        }
//        pMonthlyTrigger->Release();

//        break;
//    }
//    case ScheduleRecurrence::ONCE:
//    default:
//        pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);

//        break;
//    }

//    std::wstring taskName = getTaskName();
//    std::wstring startTime = getTaskTime();

//    std::filesystem::path exePath = std::filesystem::current_path() / "Backie.exe";
//    std::wstring nameWStr = strToWStr(this->name);
//    std::wstring typeWStr = strToWStr(strFromType(this->type));

//    std::wstring args = L"/C \"" + exePath.wstring() +
//                        L" --backup " + nameWStr + L" " + typeWStr;


//    pTrigger->put_StartBoundary(bstr_t(startTime.c_str()));

//    pExecAction->put_Path(bstr_t(L"cmd.exe"));
//    pExecAction->put_Arguments(bstr_t(args.c_str()));

//    IRegisteredTask* pRegisteredTask = NULL;
//    if (FAILED(pRootFolder->RegisterTaskDefinition(bstr_t(taskName.c_str()),
//                                                   pTask.Get(),
//                                                   TASK_CREATE_OR_UPDATE,
//                                                   _variant_t(),
//                                                   _variant_t(),
//                                                   TASK_LOGON_INTERACTIVE_TOKEN,
//                                                   _variant_t(L""),
//                                                   &pRegisteredTask)))
//        return hr;

//    CoUninitialize();
//    return hr;
//}

//bool Backup::deleteTask() {
//    if(!initializeCOM()) return false;

//    auto cleanup = [](auto &comObject)
//    {
//        if (comObject)
//            comObject->Release();
//    };

//    ComPtr<ITaskService> pService = connectToTaskScheduler();
//    if (!pService) return false;

//    ComPtr<ITaskFolder> pRootFolder;
//    HRESULT hr = pService->GetFolder(bstr_t(L"\\"), &pRootFolder);
//    if (FAILED(hr)) return false;

//    std::wstring taskName = getTaskName();

//    hr = pRootFolder->DeleteTask(bstr_t(taskName.c_str()), 0);
//    if (FAILED(hr)) return false;

//    CoUninitialize();
//    return true;
//}

