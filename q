[1mdiff --git a/backupbuilder.cpp b/backupbuilder.cpp[m
[1mindex 4b63c56..4cd9b97 100644[m
[1m--- a/backupbuilder.cpp[m
[1m+++ b/backupbuilder.cpp[m
[36m@@ -156,7 +156,7 @@[m [mstd::optional<Task> BackupBuilder::buildTask() {[m
     }[m
     for (auto& schedule : schedules) {[m
         if (!schedule->verify()) {[m
[31m-            SPDLOG_ERROR("Schedule is wrong");[m
[32m+[m[32m            SPDLOG_ERROR("Schedule of {}, is wrong", this->name);[m
             return std::nullopt;[m
         }[m
     }[m
[1mdiff --git a/destination.h b/destination.h[m
[1mindex bfa6e60..4ac9883 100644[m
[1m--- a/destination.h[m
[1m+++ b/destination.h[m
[36m@@ -7,8 +7,6 @@[m
 class Destination[m
 {[m
 public:[m
[31m-    //TODO: This construcor shouldn't be here[m
[31m-    Destination() {};[m
     Destination(std::string n, std::filesystem::path destFldr)[m
         : name{n}, destinationFolder{destFldr}, key{generate_uuid_v4()} {}[m
     Destination(std::string n, std::filesystem::path destFldr, std::string k)[m
[1mdiff --git a/errand.cpp b/errand.cpp[m
[1mindex 9260afc..bfd35ce 100644[m
[1m--- a/errand.cpp[m
[1m+++ b/errand.cpp[m
[36m@@ -96,8 +96,6 @@[m [mbool Errand::perform() const{[m
     }[m
 }[m
 [m
[31m-[m
[31m-//TODO: Check if the destination exists[m
 bool Errand::full() const {[m
     Settings& settings = Settings::getInstance();[m
     for (const auto& destinationKey : this->destinations) {[m
[36m@@ -105,6 +103,9 @@[m [mbool Errand::full() const {[m
         if (!dest) {[m
             SPDLOG_WARN("One of the destinations doesn't exist");[m
             continue;[m
[32m+[m[32m        } else if (!fs::exists(dest->destinationFolder)) {[m
[32m+[m[32m            SPDLOG_WARN("One of the destination folders doesn't exist");[m
[32m+[m[32m            continue;[m
         }[m
         fs::path backupFolder = dest->destinationFolder / this->name / formatDirName(this->currentType);[m
 [m
[36m@@ -143,6 +144,9 @@[m [mbool Errand::incremental() const {[m
         if (!dest) {[m
             SPDLOG_WARN("One of the destinations doesn't exist");[m
             continue;[m
[32m+[m[32m        } else if (!fs::exists(dest->destinationFolder)) {[m
[32m+[m[32m            SPDLOG_WARN("One of the destination folders doesn't exist");[m
[32m+[m[32m            continue;[m
         }[m
 [m
         std::optional<std::map<fs::path, FileMetadata>> combinedMetadata = loadCombinedMdMap(dest->destinationFolder);[m
[1mdiff --git a/main.cpp b/main.cpp[m
[1mindex d499569..f744fff 100644[m
[1m--- a/main.cpp[m
[1m+++ b/main.cpp[m
[36m@@ -46,7 +46,8 @@[m [mint backupWorker(int argc, char* argv[]) {[m
                             .buildErrand();[m
 [m
     if (errand) {[m
[31m-        errand->perform();[m
[32m+[m[32m        if (connected && errand->perform())[m
[32m+[m[32m        { socket.write("Performed a backup"); }[m
     } else {[m
         if (connected) { socket.write("Couldn't create backup errand"); }[m
         return 1;[m
[36m@@ -103,10 +104,10 @@[m [mnamespace Test {[m
         daily->minute = 0;[m
 [m
         std::shared_ptr<MonthlySchedule> monthlyTest = std::make_shared<MonthlySchedule>();[m
[31m-        monthly->type = BackupType::FULL;[m
[31m-        monthly->day = 31;[m
[31m-        monthly->hour = 9;[m
[31m-        monthly->minute = 0;[m
[32m+[m[32m        monthlyTest->type = BackupType::FULL;[m
[32m+[m[32m        monthlyTest->day = 31;[m
[32m+[m[32m        monthlyTest->hour = 9;[m
[32m+[m[32m        monthlyTest->minute = 0;[m
 [m
         BackupBuilder builder;[m
         auto test_task1 = builder[m
[36m@@ -146,6 +147,7 @@[m [mnamespace Test {[m
         test_task2->saveLocal();[m
         test_task3->saveLocal();[m
         test_task4->saveLocal();[m
[32m+[m[32m        test_task5->saveLocal();[m
     }[m
 [m
     void getPrintSettings() {[m
[36m@@ -188,22 +190,46 @@[m [mint guiMain(int argc, char* argv[]) {[m
     QApplication app(argc, argv);[m
     MainWindow mainWindow;[m
 [m
[32m+[m[32m    Test::cleanSettings();[m
[32m+[m
[32m+[m[32m    const std::time_t now = time(0);[m
[32m+[m[32m    const std::tm time = *std::localtime(std::addressof(now));[m
[32m+[m
[32m+[m[32m    std::shared_ptr<OnceSchedule> onceFull = std::make_shared<OnceSchedule>();[m
[32m+[m[32m    onceFull->type = BackupType::FULL;[m
[32m+[m[32m    onceFull->year = 2023;[m
[32m+[m[32m    onceFull->month = 9;[m
[32m+[m[32m    onceFull->day = 10;[m
[32m+[m[32m    onceFull->hour = time.tm_hour;[m
[32m+[m[32m    onceFull->minute = time.tm_min + 1;[m
[32m+[m
[32m+[m[32m    std::shared_ptr<OnceSchedule> onceIncremental = std::make_shared<OnceSchedule>();[m
[32m+[m[32m    onceIncremental->type = BackupType::INCREMENTAL;[m
[32m+[m[32m    onceIncremental->year = 2023;[m
[32m+[m[32m    onceIncremental->month = 9;[m
[32m+[m[32m    onceIncremental->day = 10;[m
[32m+[m[32m    onceIncremental->hour = time.tm_hour;[m
[32m+[m[32m    onceIncremental->minute = time.tm_min + 2;[m
 [m
[31m-//    BackupBuilder builder;[m
[31m-//    auto errand = builder[m
[31m-//                      .setKey("1d16ce4f-e996-429b-a3c9-bcb1222f1d14")[m
[31m-//                      .setCurrentType(BackupType::FULL)[m
[31m-//                      .buildErrand();[m
[32m+[m[32m    Settings& settings = Settings::getInstance();[m
[32m+[m[32m    Destination test_dest1("Default destination 1", "W:\\Backie backups\\Dest 1");[m
[32m+[m[32m    settings.addUpdate(test_dest1);[m
 [m
[31m-//    if (errand) {[m
[31m-//        errand->perform();[m
[31m-//        SPDLOG_INFO("Performed a backup");[m
[31m-//    } else {[m
[31m-//        SPDLOG_ERROR("Error");[m
[31m-//    }[m
[32m+[m[32m    BackupBuilder builder;[m
[32m+[m[32m    auto test_task = builder[m
[32m+[m[32m                    .setName("Current test")[m
[32m+[m[32m                    .setSchedules({onceFull, onceIncremental})[m
[32m+[m[32m                    .setDestinations({test_dest1})[m
[32m+[m[32m                    .setSources({"W:\\Src folder 1"})[m
[32m+[m[32m                    .buildTask();[m
[32m+[m
[32m+[m[32m    if (test_task) {[m
[32m+[m[32m        test_task->saveLocal();[m
[32m+[m[32m    } else {[m
[32m+[m[32m        SPDLOG_ERROR("Couldn't create the task");[m
[32m+[m[32m    }[m
 [m
 [m
[31m-    Test::cleanSettings();[m
 [m
 //    Test::populateSettings();[m
 [m
[1mdiff --git a/schedule.cpp b/schedule.cpp[m
[1mindex 82e9d7f..c7d8384 100644[m
[1m--- a/schedule.cpp[m
[1m+++ b/schedule.cpp[m
[36m@@ -44,8 +44,8 @@[m [mnlohmann::json MonthlySchedule::toJson() const {[m
     };[m
 }[m
 bool MonthlySchedule::verify() const {[m
[31m-    return !(hour < 0 || hour > 23 || minute < 0 || minute > 59 ||[m
[31m-            !(day == -1 || day > 0) || day > 31);[m
[32m+[m[32m    return !(hour < 0 || hour > 23 || minute < 0 || minute > 59 || day > 31)[m
[32m+[m[32m           && (day == -1 || day > 0);[m
 }[m
 [m
 nlohmann::json WeeklySchedule::toJson() const  {[m
[1mdiff --git a/settings.cpp b/settings.cpp[m
[1mindex e6ea19d..c1f7967 100644[m
[1m--- a/settings.cpp[m
[1m+++ b/settings.cpp[m
[36m@@ -160,7 +160,6 @@[m [mstd::vector<Destination> Settings::getDestVec() const {[m
     return destinations;[m
 }[m
 [m
[31m-//TODO: That's why that constructor was needed[m
 std::optional<Destination> Settings::getDest(const std::string& key) const {[m
     if (data.find("destinations") == data.end()) {[m
         SPDLOG_WARN("No destinations found");[m
[1mdiff --git a/task.cpp b/task.cpp[m
[1mindex 9306305..aac8fc0 100644[m
[1m--- a/task.cpp[m
[1m+++ b/task.cpp[m
[36m@@ -42,13 +42,16 @@[m [mstd::ostream & operator << (std::ostream &out, const Task &task) {[m
     return out;[m
 }[m
 [m
[31m-//TODO: Save changes if somethings was deleted[m
[32m+[m[32m//TODO: Just deleting everything and adding it again isn't the best solution[m
 bool Task::saveLocal() const {[m
[31m-    for (auto& schedule : this->schedules){[m
[31m-        if (FAILED(this->saveTaskScheduler(*schedule))) {[m
[31m-            return false;[m
[31m-        }[m
[32m+[m[32m    if (FAILED(this->deleteTaskScheduler())) {[m
[32m+[m[32m        return false;[m
[32m+[m[32m    }[m
[32m+[m[32m//    for (auto& schedule : this->schedules){[m
[32m+[m[32m    if (FAILED(this->saveTaskScheduler())) {[m
[32m+[m[32m        return false;[m
     }[m
[32m+[m[32m//    }[m
 [m
     Settings& settings = Settings::getInstance();[m
 [m
[36m@@ -64,36 +67,37 @@[m [mbool Task::deleteLocal() const {[m
     return settings.remove(*this);[m
 }[m
 [m
[31m-static std::wstring getTaskName(BackupType type, ScheduleRecurrence recurrence, const std::string& key) {[m
[31m-    std::wstring recurrenceString;[m
[31m-    switch (recurrence) {[m
[31m-    case ScheduleRecurrence::ONCE:[m
[31m-        recurrenceString = L"singular";[m
[31m-        break;[m
[31m-    case ScheduleRecurrence::MONTHLY:[m
[31m-        recurrenceString = L"monthly";[m
[31m-        break;[m
[31m-    case ScheduleRecurrence::WEEKLY:[m
[31m-        recurrenceString = L"weekly";[m
[31m-        break;[m
[31m-    case ScheduleRecurrence::DAILY:[m
[31m-        recurrenceString = L"daily";[m
[31m-        break;[m
[31m-    }[m
[31m-[m
[31m-    std::wstring typeString = strToWStr(strFromType(type));[m
[31m-[m
[31m-    //TODO: Maybe new name shouldn't be crated[m
[31m-    std::wstring taskName;[m
[31m-    taskName = typeString + L" " + recurrenceString + L" " + strToWStr(key);[m
[31m-[m
[31m-    // Cleanup for task scheduler rules[m
[31m-    std::replace(taskName.begin(), taskName.end(), '/', ' ');[m
[31m-    std::replace(taskName.begin(), taskName.end(), '\\', ' ');[m
[31m-    taskName.erase(std::remove(taskName.begin(), taskName.end(), ':'), taskName.end());[m
[31m-[m
[31m-    return taskName;[m
[31m-}[m
[32m+[m[32m//static std::wstring getTaskName(BackupType type, ScheduleRecurrence recurrence, const std::string& key) {[m
[32m+[m[32m//    std::wstring recurrenceString;[m
[32m+[m[32m//    switch (recurrence) {[m
[32m+[m[32m//    case ScheduleRecurrence::ONCE:[m
[32m+[m[32m//        recurrenceString = L"singular";[m
[32m+[m[32m//        break;[m
[32m+[m[32m//    case ScheduleRecurrence::MONTHLY:[m
[32m+[m[32m//        recurrenceString = L"monthly";[m
[32m+[m[32m//        break;[m
[32m+[m[32m//    case ScheduleRecurrence::WEEKLY:[m
[32m+[m[32m//        recurrenceString = L"weekly";[m
[32m+[m[32m//        break;[m
[32m+[m[32m//    case ScheduleRecurrence::DAILY:[m
[32m+[m[32m//        recurrenceString = L"daily";[m
[32m+[m[32m//        break;[m
[32m+[m[32m//    }[m
[32m+[m
[32m+[m[32m//    std::wstring typeString = ;[m
[32m+[m
[32m+[m[32m//    //TODO: Maybe new name shouldn't be crated[m
[32m+[m[32m//    std::wstring taskName;[m
[32m+[m[32m////    taskName = typeString + L" " + recurrenceString + L" " + strToWStr(key);[m
[32m+[m[32m//    taskName = strToWStr(key);[m
[32m+[m
[32m+[m[32m//    // Cleanup for task scheduler rules[m
[32m+[m[32m//    std::replace(taskName.begin(), taskName.end(), '/', ' ');[m
[32m+[m[32m//    std::replace(taskName.begin(), taskName.end(), '\\', ' ');[m
[32m+[m[32m//    taskName.erase(std::remove(taskName.begin(), taskName.end(), ':'), taskName.end());[m
[32m+[m
[32m+[m[32m//    return taskName;[m
[32m+[m[32m//}[m
 [m
 static std::wstring getTaskTime(int year, int month, int day, int hour, int minute) {[m
     std::wstring startTime;[m
[36m@@ -145,17 +149,11 @@[m [mstatic HRESULT initializeCOM(){[m
     return hr;[m
 }[m
 [m
[31m-HRESULT Task::saveTaskScheduler(Schedule& schedule) const {[m
[32m+[m[32mHRESULT Task::saveTaskScheduler() const {[m
     HRESULT hr = initializeCOM();[m
 [m
     if (FAILED(hr)) return hr;[m
 [m
[31m-    auto cleanup = [](auto &comObject)[m
[31m-    {[m
[31m-        if (comObject)[m
[31m-            comObject->Release();[m
[31m-    };[m
[31m-[m
     ComPtr<ITaskService> pService = connectToTaskScheduler();[m
     if (!pService) return hr;[m
 [m
[36m@@ -163,102 +161,112 @@[m [mHRESULT Task::saveTaskScheduler(Schedule& schedule) const {[m
     hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);[m
     if (FAILED(hr)) return hr;[m
 [m
[31m-    ComPtr<ITaskDefinition> pTask;[m
[31m-    hr = pService->NewTask(0, &pTask);[m
[31m-    if (FAILED(hr)) return hr;[m
 [m
[31m-    ComPtr<ITriggerCollection> pTriggerCollection;[m
[31m-    hr = pTask->get_Triggers(&pTriggerCollection);[m
[31m-    if (FAILED(hr)) return hr;[m
[32m+[m[32m    // Group schedules by their type[m
[32m+[m[32m    std::map<BackupType, std::vector<std::shared_ptr<Schedule>>> groupedSchedules;[m
[32m+[m[32m    for (const auto& schedule : schedules) {[m
[32m+[m[32m        groupedSchedules[schedule->type].push_back(schedule);[m
[32m+[m[32m    }[m
 [m
[31m-    ComPtr<IActionCollection> pActionCollection;[m
[31m-    hr = pTask->get_Actions(&pActionCollection);[m
[31m-    if (FAILED(hr)) return hr;[m
[32m+[m[32m    for (const auto& [type, schedulesOfType] : groupedSchedules) {[m
 [m
[31m-    ComPtr<IAction> pAction;[m
[31m-    hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);[m
[31m-    if (FAILED(hr)) return hr;[m
[32m+[m[32m        ComPtr<ITaskDefinition> pTask;[m
[32m+[m[32m        hr = pService->NewTask(0, &pTask);[m
[32m+[m[32m        if (FAILED(hr)) return hr;[m
 [m
[31m-    ComPtr<IExecAction> pExecAction;[m
[31m-    hr = pAction->QueryInterface(IID_IExecAction, (void **)&pExecAction);[m
[31m-    if (FAILED(hr)) return hr;[m
 [m
[31m-    ITrigger* pTrigger = NULL;[m
[32m+[m[32m        ComPtr<IActionCollection> pActionCollection;[m
[32m+[m[32m        hr = pTask->get_Actions(&pActionCollection);[m
[32m+[m[32m        if (FAILED(hr)) return hr;[m
 [m
[31m-    std::wstring startTime;[m
[31m-    switch (schedule.getRecurrence())[m
[31m-    {[m
[31m-    case ScheduleRecurrence::DAILY: {[m
[31m-        const DailySchedule& daily = dynamic_cast<const DailySchedule&>(schedule);[m
[31m-        startTime = getTaskTime(daily.hour, daily.minute);[m
[32m+[m[32m        ComPtr<IAction> pAction;[m
[32m+[m[32m        hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);[m
[32m+[m[32m        if (FAILED(hr)) return hr;[m
 [m
[31m-        pTriggerCollection->Create(TASK_TRIGGER_DAILY, &pTrigger);[m
[31m-        break;[m
[31m-    }[m
[31m-    case ScheduleRecurrence::WEEKLY: {[m
[31m-        const WeeklySchedule& weekly = dynamic_cast<const WeeklySchedule&>(schedule);[m
[31m-        startTime = getTaskTime(weekly.hour, weekly.minute);[m
[32m+[m[32m        ComPtr<IExecAction> pExecAction;[m
[32m+[m[32m        hr = pAction->QueryInterface(IID_IExecAction, (void **)&pExecAction);[m
[32m+[m[32m        if (FAILED(hr)) return hr;[m
 [m
[31m-        pTriggerCollection->Create(TASK_TRIGGER_WEEKLY, &pTrigger);[m
[31m-        IWeeklyTrigger* pWeeklyTrigger = NULL;[m
[31m-        pTrigger->QueryInterface(IID_IWeeklyTrigger, (void**)&pWeeklyTrigger);[m
[31m-        pWeeklyTrigger->put_DaysOfWeek(1 << (weekly.day - 1));[m
[31m-        pWeeklyTrigger->Release();[m
[32m+[m[32m        std::filesystem::path exePath = std::filesystem::current_path() / "Backie.exe";[m
[32m+[m[32m        std::wstring keyWStr = strToWStr(this->key);[m
[32m+[m[32m        std::wstring typeWStr = strToWStr(strFromType(type));[m
 [m
[31m-        break;[m
[31m-    }[m
[31m-    case ScheduleRecurrence::MONTHLY: {[m
[31m-        const MonthlySchedule& monthly = dynamic_cast<const MonthlySchedule&>(schedule);[m
[31m-        startTime = getTaskTime(monthly.hour, monthly.minute);[m
[31m-[m
[31m-        pTriggerCollection->Create(TASK_TRIGGER_MONTHLY, &pTrigger);[m
[31m-        IMonthlyTrigger* pMonthlyTrigger = NULL;[m
[31m-        pTrigger->QueryInterface(IID_IMonthlyTrigger, (void**)&pMonthlyTrigger);[m
[31m-        if (monthly.day > 0) {[m
[31m-            pMonthlyTrigger->put_DaysOfMonth(1 << (monthly.day - 1));[m
[31m-        } else {[m
[31m-            pMonthlyTrigger->put_RunOnLastDayOfMonth(true);[m
[31m-        }[m
[31m-        pMonthlyTrigger->Release();[m
[32m+[m[32m        std::wstring args = L"/C \"" + exePath.wstring() +[m
[32m+[m[32m                            L" --backup " + keyWStr + L" " + typeWStr;[m
 [m
[31m-        break;[m
[31m-    }[m
[31m-    case ScheduleRecurrence::ONCE: {[m
[31m-        const OnceSchedule& once = dynamic_cast<const OnceSchedule&>(schedule);[m
[31m-        startTime = getTaskTime(once.year, once.month, once.day, once.hour, once.minute);[m
[32m+[m[32m        pExecAction->put_Path(bstr_t(L"cmd.exe"));[m
[32m+[m[32m        pExecAction->put_Arguments(bstr_t(args.c_str()));[m
 [m
[31m-        pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);[m
[31m-        break;[m
[31m-    }[m
[31m-    default:[m
[31m-        break;[m
[31m-    }[m
[32m+[m[32m        for (const auto& schedule : schedulesOfType) {[m
[32m+[m[32m            ITrigger* pTrigger = NULL;[m
 [m
[31m-    std::filesystem::path exePath = std::filesystem::current_path() / "Backie.exe";[m
[31m-    std::wstring keyWStr = strToWStr(this->key);[m
[31m-    std::wstring typeWStr = strToWStr(strFromType(schedule.type));[m
[32m+[m[32m            ComPtr<ITriggerCollection> pTriggerCollection;[m
[32m+[m[32m            hr = pTask->get_Triggers(&pTriggerCollection);[m
[32m+[m[32m            if (FAILED(hr)) return hr;[m
 [m
[31m-    std::wstring args = L"/C \"" + exePath.wstring() +[m
[31m-                        L" --backup " + keyWStr + L" " + typeWStr;[m
[32m+[m[32m            std::wstring startTime;[m
[32m+[m[32m            switch (schedule->getRecurrence())[m
[32m+[m[32m            {[m
[32m+[m[32m            case ScheduleRecurrence::DAILY: {[m
[32m+[m[32m                const DailySchedule& daily = dynamic_cast<const DailySchedule&>(*schedule);[m
[32m+[m[32m                startTime = getTaskTime(daily.hour, daily.minute);[m
 [m
[32m+[m[32m                pTriggerCollection->Create(TASK_TRIGGER_DAILY, &pTrigger);[m
[32m+[m[32m                break;[m
[32m+[m[32m            }[m
[32m+[m[32m            case ScheduleRecurrence::WEEKLY: {[m
[32m+[m[32m                const WeeklySchedule& weekly = dynamic_cast<const WeeklySchedule&>(*schedule);[m
[32m+[m[32m                startTime = getTaskTime(weekly.hour, weekly.minute);[m
 [m
[31m-    std::wstring taskName = getTaskName(schedule.type, schedule.getRecurrence(), this->key);[m
[32m+[m[32m                pTriggerCollection->Create(TASK_TRIGGER_WEEKLY, &pTrigger);[m
[32m+[m[32m                IWeeklyTrigger* pWeeklyTrigger = NULL;[m
[32m+[m[32m                pTrigger->QueryInterface(IID_IWeeklyTrigger, (void**)&pWeeklyTrigger);[m
[32m+[m[32m                pWeeklyTrigger->put_DaysOfWeek(1 << (weekly.day - 1));[m
[32m+[m[32m                pWeeklyTrigger->Release();[m
 [m
[31m-    pTrigger->put_StartBoundary(bstr_t(startTime.c_str()));[m
[32m+[m[32m                break;[m
[32m+[m[32m            }[m
[32m+[m[32m            case ScheduleRecurrence::MONTHLY: {[m
[32m+[m[32m                const MonthlySchedule& monthly = dynamic_cast<const MonthlySchedule&>(*schedule);[m
[32m+[m[32m                startTime = getTaskTime(monthly.hour, monthly.minute);[m
[32m+[m
[32m+[m[32m                pTriggerCollection->Create(TASK_TRIGGER_MONTHLY, &pTrigger);[m
[32m+[m[32m                IMonthlyTrigger* pMonthlyTrigger = NULL;[m
[32m+[m[32m                pTrigger->QueryInterface(IID_IMonthlyTrigger, (void**)&pMonthlyTrigger);[m
[32m+[m[32m                if (monthly.day > 0) {[m
[32m+[m[32m                    pMonthlyTrigger->put_DaysOfMonth(1 << (monthly.day - 1));[m
[32m+[m[32m                } else {[m
[32m+[m[32m                    pMonthlyTrigger->put_RunOnLastDayOfMonth(true);[m
[32m+[m[32m                }[m
[32m+[m[32m                pMonthlyTrigger->Release();[m
 [m
[31m-    pExecAction->put_Path(bstr_t(L"cmd.exe"));[m
[31m-    pExecAction->put_Arguments(bstr_t(args.c_str()));[m
[32m+[m[32m                break;[m
[32m+[m[32m            }[m
[32m+[m[32m            case ScheduleRecurrence::ONCE: {[m
[32m+[m[32m                const OnceSchedule& once = dynamic_cast<const OnceSchedule&>(*schedule);[m
[32m+[m[32m                startTime = getTaskTime(once.year, once.month, once.day, once.hour, once.minute);[m
 [m
[31m-    IRegisteredTask* pRegisteredTask = NULL;[m
[31m-    if (FAILED(pRootFolder->RegisterTaskDefinition(bstr_t(taskName.c_str()),[m
[31m-                                                   pTask.Get(),[m
[31m-                                                   TASK_CREATE_OR_UPDATE,[m
[31m-                                                   _variant_t(),[m
[31m-                                                   _variant_t(),[m
[31m-                                                   TASK_LOGON_INTERACTIVE_TOKEN,[m
[31m-                                                   _variant_t(L""),[m
[31m-                                                   &pRegisteredTask)))[m
[31m-        return hr;[m
[32m+[m[32m                pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);[m
[32m+[m[32m                break;[m
[32m+[m[32m            }[m
[32m+[m[32m            default:[m
[32m+[m[32m                break;[m
[32m+[m[32m            }[m
[32m+[m[32m            pTrigger->put_StartBoundary(bstr_t(startTime.c_str()));[m
[32m+[m[32m        }[m
[32m+[m
[32m+[m[32m        std::wstring taskName = L"Backie " + strToWStr(strFromType(type)) + L" " + strToWStr(this->key);[m
[32m+[m[32m        IRegisteredTask* pRegisteredTask = NULL;[m
[32m+[m[32m        if (FAILED(pRootFolder->RegisterTaskDefinition(bstr_t(taskName.c_str()),[m
[32m+[m[32m                                                       pTask.Get(),[m
[32m+[m[32m                                                       TASK_CREATE_OR_UPDATE,[m
[32m+[m[32m                                                       _variant_t(),[m
[32m+[m[32m                                                       _variant_t(),[m
[32m+[m[32m                                                       TASK_LOGON_INTERACTIVE_TOKEN,[m
[32m+[m[32m                                                       _variant_t(L""),[m
[32m+[m[32m                                                       &pRegisteredTask)))[m
[32m+[m[32m            return hr;[m
[32m+[m[32m    }[m
 [m
     CoUninitialize();[m
     return hr;[m
[36m@@ -267,12 +275,6 @@[m [mHRESULT Task::saveTaskScheduler(Schedule& schedule) const {[m
 HRESULT Task::deleteTaskScheduler() const {[m
     HRESULT hr = initializeCOM();[m
 [m
[31m-    auto cleanup = [](auto &comObject)[m
[31m-    {[m
[31m-        if (comObject)[m
[31m-            comObject->Release();[m
[31m-    };[m
[31m-[m
     ComPtr<ITaskService> pService = connectToTaskScheduler();[m
     if (!pService) return hr;[m
 [m
[36m@@ -282,14 +284,14 @@[m [mHRESULT Task::deleteTaskScheduler() const {[m
 [m
 [m
     ComPtr<IRegisteredTaskCollection> pTaskCollection;[m
[31m-    hr = pRootFolder->GetTasks(NULL, &pTaskCollection);[m
[32m+[m[32m    hr = pRootFolder->GetTasks(0, &pTaskCollection);[m
     if (FAILED(hr)) return hr;[m
 [m
     LONG numTasks = 0;[m
     hr = pTaskCollection->get_Count(&numTasks);[m
     if (FAILED(hr)) return hr;[m
 [m
[31m-    bool deleteSuccess = true; // Assume success until proven otherwise[m
[32m+[m[32m    bool deleteSuccess = true;[m
 [m
     for (LONG i = 1; i <= numTasks; ++i) { // COM collections are usually 1-based[m
         ComPtr<IRegisteredTask> pRegisteredTask;[m
[1mdiff --git a/task.h b/task.h[m
[1mindex 302e2a9..f06aadc 100644[m
[1m--- a/task.h[m
[1m+++ b/task.h[m
[36m@@ -25,7 +25,7 @@[m [mprotected:[m
     std::vector<std::shared_ptr<Schedule>> schedules;[m
 [m
 private:[m
[31m-    HRESULT saveTaskScheduler(Schedule& schedule) const;[m
[32m+[m[32m    HRESULT saveTaskScheduler() const;[m
     HRESULT deleteTaskScheduler() const;[m
 };[m
 [m
