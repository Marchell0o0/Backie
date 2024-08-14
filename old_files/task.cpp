#include "pch.h"

#include "task.h"

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



