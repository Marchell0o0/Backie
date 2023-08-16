#include <comdef.h>
#include <taskschd.h>
#include <string>
#include <iostream>
#include <comutil.h>
#include <wrl/client.h>
#include <filesystem>
#include <algorithm>

#include "spdlog/spdlog.h"
#include "taskscheduler.h"

using Microsoft::WRL::ComPtr;


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

static bool initializeCOM(){
    HRESULT hr = CoInitialize(NULL);
    if (hr != S_OK)
    {
        if (hr == S_FALSE) {
//            SPDLOG_WARN("COM library was already initialized on this thread.");
        } else if (hr == RPC_E_CHANGED_MODE) {
            SPDLOG_ERROR("A previous call to CoInitializeEx specified a different threading model.");
            return false;
        } else {
            SPDLOG_ERROR("Couldn't initialize COM library. Error code: {}", hr);
            return false;
        }
    }
    return true;
}


bool addTask(Backup backup){

    if (!initializeCOM()) return false;

    auto cleanup = [](auto &comObject)
    {
        if (comObject)
            comObject->Release();
    };

    ComPtr<ITaskService> pService = connectToTaskScheduler();
    if (!pService) return false;

    ComPtr<ITaskFolder> pRootFolder;
    HRESULT hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr)) return false;

    ComPtr<ITaskDefinition> pTask;
    hr = pService->NewTask(0, &pTask);
    if (FAILED(hr)) return false;

    ComPtr<ITriggerCollection> pTriggerCollection;
    hr = pTask->get_Triggers(&pTriggerCollection);
    if (FAILED(hr)) return false;

    ComPtr<IActionCollection> pActionCollection;
    hr = pTask->get_Actions(&pActionCollection);
    if (FAILED(hr)) return false;

    ComPtr<IAction> pAction;
    hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
    if (FAILED(hr)) return false;

    ComPtr<IExecAction> pExecAction;
    hr = pAction->QueryInterface(IID_IExecAction, (void **)&pExecAction);
    if (FAILED(hr)) return false;

    ITrigger* pTrigger = NULL;


    switch (backup.getRecurrence())
    {
        case ScheduleRecurrence::DAILY:
            pTriggerCollection->Create(TASK_TRIGGER_DAILY, &pTrigger);

            break;

        case ScheduleRecurrence::WEEKLY: {
            pTriggerCollection->Create(TASK_TRIGGER_WEEKLY, &pTrigger);
            IWeeklyTrigger* pWeeklyTrigger = NULL;
            pTrigger->QueryInterface(IID_IWeeklyTrigger, (void**)&pWeeklyTrigger);
            pWeeklyTrigger->put_DaysOfWeek(1 << (backup.getDay() - 1));
            pWeeklyTrigger->Release();

            break;
        }
        case ScheduleRecurrence::MONTHLY: {
            pTriggerCollection->Create(TASK_TRIGGER_MONTHLY, &pTrigger);
            IMonthlyTrigger* pMonthlyTrigger = NULL;
            pTrigger->QueryInterface(IID_IMonthlyTrigger, (void**)&pMonthlyTrigger);
            pMonthlyTrigger->put_DaysOfMonth(1 << (backup.getDay() - 1));
            pMonthlyTrigger->Release();

            break;
        }
        case ScheduleRecurrence::ONCE:
        default:
            pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);

            break;
    }


    std::string taskName = backup.getTaskName();
    std::wstring startTime = backup.getTaskTime();

    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path exePath = currentPath / "Backie.exe";

//    SPDLOG_INFO("Task name {}", taskName);
//    SPDLOG_INFO("Executable directory {}", exePath.u8string());

    BSTR bstrStartTime = SysAllocString(startTime.c_str());
    if (!bstrStartTime) {
        SPDLOG_ERROR("Couldn't allocate BSTR for startTime");
        return false;
    }

    BSTR bstrExePath = SysAllocString(exePath.c_str());
    if (!bstrExePath) {
        SPDLOG_ERROR("Couldn't allocate BSTR for exePath");
        SysFreeString(bstrStartTime);
        return false;
    }

    BSTR bstrTaskName = SysAllocString(std::wstring(taskName.begin(), taskName.end()).c_str());
    if (!bstrTaskName) {
        SPDLOG_ERROR("Couldn't allocate BSTR for taskName");
        SysFreeString(bstrStartTime);
        SysFreeString(bstrExePath);
        return false;
    }

    pTrigger->put_StartBoundary(bstrStartTime);

    pExecAction->put_Path(bstrExePath);

    IRegisteredTask* pRegisteredTask = NULL;
    if (FAILED(pRootFolder->RegisterTaskDefinition(bstrTaskName,
                                           pTask.Get(),
                                           TASK_CREATE_OR_UPDATE,
                                           _variant_t(),
                                           _variant_t(),
                                           TASK_LOGON_INTERACTIVE_TOKEN,
                                           _variant_t(L""),
                                                   &pRegisteredTask)))
        return false;

    SysFreeString(bstrTaskName);
    SysFreeString(bstrStartTime);
    SysFreeString(bstrExePath);

    CoUninitialize();
    return true;
    }

bool deleteTask(Backup backup){
    if(!initializeCOM()) return false;

    auto cleanup = [](auto &comObject)
    {
        if (comObject)
            comObject->Release();
    };

    ComPtr<ITaskService> pService = connectToTaskScheduler();
    if (!pService) return false;

    // Get the root folder
    ComPtr<ITaskFolder> pRootFolder;
    HRESULT hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr)) return false;


    std::string taskName = backup.getTaskName();

    // Convert the cleaned up task name to a wide string for the DeleteTask method
    BSTR bstrCleanTaskName = SysAllocString(std::wstring(taskName.begin(), taskName.end()).c_str());
    hr = pRootFolder->DeleteTask(bstrCleanTaskName, 0);
    if (FAILED(hr)) return false;

    CoUninitialize();
    return true;
}
