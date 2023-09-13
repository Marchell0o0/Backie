#include <QFile>
#include <QFileDialog>
#include <QButtonGroup>
#include <iostream>

#include "backupbuilder.h"
#include "settings.h"
#include "spdlog/spdlog.h"
#include "backupscreen.h"
#include "ui_backupscreen.h"
#include "dialogcalendar.h"


BackupScreen::BackupScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BackupScreen)
{
    ui->setupUi(this);

    dayOfWeekMap = {
        {1, "Monday"},
        {2, "Tuesday"},
        {3, "Wednesday"},
        {4, "Thursday"},
        {5, "Friday"},
        {6, "Saturday"},
        {7, "Sunday"}
    };

    loadStyleSheet(":/styles/backupScreen.css", ui->stackedWidget);

    connect(ui->browseSourceB, &QPushButton::clicked, this, &BackupScreen::browseForSourceFolder);

    initRecurrAndDateBtns();
    initDaysOfWeekBtns();

    connect(ui->createBackupB, &QPushButton::clicked, this, &BackupScreen::createBackupBclicked);
}

BackupScreen::~BackupScreen()
{
    delete ui;
}

void BackupScreen::initRecurrAndDateBtns() {
    // Initialize RadioButtons
    QList<QPair<QRadioButton*, ScheduleRecurrence>> radioButtonMapping = {
       // TODO: add NOW and ONCE A YEAR types
       {ui->OnceDateRB, ScheduleRecurrence::ONCE},
       {ui->OnceDayRB, ScheduleRecurrence::DAILY},
       {ui->OnceWeekRB, ScheduleRecurrence::WEEKLY},
       {ui->OnceMonthRB, ScheduleRecurrence::MONTHLY},
       };
    connectRecurrAndDateBtns(radioButtonMapping);

    // Initialize ComboBoxes
    QList<QPair<QComboBox*, BackupArgs::DateArgs>> comboBoxMapping = {
        {ui->selectMonthCB, BackupArgs::MONTH},
        {ui->selectDayCB, BackupArgs::DAYOFMONTH},
        {ui->selectHourCB, BackupArgs::HOUR},
        {ui->selectMinuteCB, BackupArgs::MINUTE}
    };
    connectRecurrAndDateBtns(comboBoxMapping);
}

template <typename T, typename U>
void BackupScreen::connectRecurrAndDateBtns(const QList<QPair<T*, U>>& mapping) {
    for (const auto& pair : mapping) {
        T* widget = pair.first;
        U type = pair.second;

        widget->setProperty("type", static_cast<int>(type));

        if (auto rb = dynamic_cast<QRadioButton*>(widget)) {
            connect(rb, &QRadioButton::toggled, this, &BackupScreen::handleRadioButtonToggle);
        } else if (auto cb = dynamic_cast<QComboBox*>(widget)) {
            connect(cb, &QComboBox::currentIndexChanged, this, &BackupScreen::handleComboBoxIndexChanged);
        }
    }
}

void BackupScreen::initDaysOfWeekBtns() {
    daysOfWeekBtnGroup = new QButtonGroup(this);

    QWidget* daysOfWeekWidget = this->findChild<QWidget*>("selectWeekPage");
    if (daysOfWeekWidget) {
        QList<QPushButton*> buttons = daysOfWeekWidget->findChildren<QPushButton*>();
        std::sort(buttons.begin(), buttons.end(), [](QPushButton* a, QPushButton* b) {
            return a->objectName() < b->objectName();
        });

        for (int i = 0; i < buttons.size(); ++i) {
            daysOfWeekBtnGroup->addButton(buttons[i], i);
        }
    }
    connect(daysOfWeekBtnGroup, SIGNAL(buttonToggled(QAbstractButton*,bool)), this, SLOT(handleDayOfWeekButtonClicked(QAbstractButton*, bool)));
}

void BackupScreen::browseForSourceFolder() {
    //TODO: decide whether to resolve symbol links
    QString chosenDir = QFileDialog::getExistingDirectory(this, tr("Choose a folder to backup"),
                                                          QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!chosenDir.isEmpty()) {
        ui->sourceEdit->setText(QString(chosenDir));
        #ifdef _WIN32  // for Windows
        // TODO: write set function
        backupArgs.setSourcePath(std::filesystem::path(chosenDir.toStdWString()));
        #endif
        // later we can add for MacOS or Linux here
        SPDLOG_DEBUG("Directory for backup was chosen successfuly: {}", chosenDir.toStdString());
    } else {
        //handle an error
        SPDLOG_ERROR("Directory for backup was not chosen successfuly");
    }
}

void BackupScreen::onSourcePathEdited(const QString& newPath){
    #ifdef _WIN32  // for Windows
    backupArgs.setSourcePath(std::filesystem::path(newPath.toStdString()));
    #endif
    SPDLOG_DEBUG("Directory for backup was changed successfuly: {}", newPath.toStdString());
}

void BackupScreen::on_selectDateB_clicked()
{
    dialogCalendar dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QDate date = dialog.selectedDate();

        backupArgs.setDate(BackupArgs::YEAR, date.year());
        backupArgs.setDate(BackupArgs::MONTH, date.month());
        backupArgs.setDate(BackupArgs::DAYOFMONTH, date.day());

        // TODO: add selectMonthCB updating for ONCE A YEAR option
        ui->selectDayCB->setCurrentText(QString::number(backupArgs.getDate(BackupArgs::DAYOFMONTH)));

        updateChosenDateLabel();
    }
    else {
        SPDLOG_DEBUG("Date not accepted");
    }
}


// implement date selection logic
void BackupScreen::handleRadioButtonToggle(bool isChecked){
    QRadioButton* button = qobject_cast<QRadioButton*>(sender());
    if (button) {
        // ensure one button is always checked
        if (!isChecked) {
            button->blockSignals(true);
            button->setChecked(true);
            button->blockSignals(false);
            return;
        }
        backupArgs.setBackupRecurrence(static_cast<ScheduleRecurrence>(
            button->property("type").toInt()));

        switch (backupArgs.getBackupRecurrence()) {

              // NOW TYPE

//            case NowType:
//                // widget all selection disabled
//                ui->holderStackedWidget->setCurrentIndex(0);
//                if (ui->selectDateB->isEnabled()) {
//                    ui->selectDateB->setDisabled(isChecked);
//                }
//                ui->createBackupScheduleB->setText(QString("Make a backup"));
//                break;
        case ScheduleRecurrence::ONCE:
                // widget with calendar selection
                ui->holderStackedWidget->setCurrentIndex(0);
                if (!ui->selectDateB->isEnabled()) {
                    ui->selectDateB->setEnabled(isChecked);
                }
                ui->createBackupB->setText(QString("Create a backup task"));
                break;
        case ScheduleRecurrence::DAILY:
                // widget date selection disabled
                ui->holderStackedWidget->setCurrentIndex(0);
                if (ui->selectDateB->isEnabled()) {
                    ui->selectDateB->setDisabled(isChecked);
                }
                ui->createBackupB->setText(QString("Create a backup schedule"));
                break;
        case ScheduleRecurrence::WEEKLY:
                // widget with days of week selection
                ui->holderStackedWidget->setCurrentIndex(2);
                ui->createBackupB->setText(QString("Create a backup schedule"));
                break;
        case ScheduleRecurrence::MONTHLY:
                // widget with month selection
                ui->holderStackedWidget->setCurrentIndex(1);
                if (ui->selectMonthCB->isVisible()) {
                    ui->selectMonthCB->setVisible(!isChecked);
                }
                ui->createBackupB->setText(QString("Create a backup schedule"));
                backupArgs.setDate(BackupArgs::DAYOFMONTH, ui->selectDayCB->currentText().toInt());
                break;


              // ONCE A YEAR CASE

//            case OnceYearType:
//                ui->holderStackedWidget->setCurrentIndex(1);
//                if (!ui->selectMonthCB->isVisible()) {
//                    ui->selectMonthCB->setVisible(isChecked);
//                }
//                ui->createBackupScheduleB->setText(QString("Create a backup schedule"));
//                break;
        }
        backupArgs.setDate(BackupArgs::HOUR, ui->selectHourCB->currentText().toInt());
        backupArgs.setDate(BackupArgs::MINUTE, ui->selectMinuteCB->currentText().toInt());

        updateChosenDateLabel();
    }
}

void BackupScreen::handleComboBoxIndexChanged(int index) {
    if (index < 0) {
        SPDLOG_ERROR("handleComboBoxIndexChanged() catched invalid index");
        return;  // Invalid index
    }
    QComboBox* comboBox = qobject_cast<QComboBox*>(sender());

    if (!comboBox) return;

    BackupArgs::DateArgs argType = static_cast<BackupArgs::DateArgs>(
        comboBox->property("type").toInt());

    backupArgs.setDate(argType, comboBox->currentText().toInt());

    updateChosenDateLabel();
}

void BackupScreen::handleDayOfWeekButtonClicked(QAbstractButton* button, bool isChecked) {
     int dayOfWeek = daysOfWeekBtnGroup->id(button);
    if (isChecked) {
        //dayOfWeek: 0...6, dayOfWeek + 1: -> 1...7
        backupArgs.setDate(BackupArgs::DAYOFWEEK, dayOfWeek + 1);
    }
    updateChosenDateLabel();
}

QString BackupScreen::formatDate(int day, int month, int year, int hour, int minute) {
    return QString("%1.%2.%3 at %4:%5")
        .arg(addZero(day),
             addZero(month),
             QString::number(year),
             addZero(hour),
             addZero(minute));
}

QString BackupScreen::getTimeString(int hour, int minute) {
    return QString("%1:%2")
        .arg(addZero(hour), addZero(minute));
}

void BackupScreen::updateChosenDateLabel() {
    auto date = backupArgs;

    if (date.getDate(BackupArgs::HOUR) == -1 || date.getDate(BackupArgs::MINUTE) == -1) {
        ui->chosenDateL->setText("Chosen date is...");
        return;
    }

    QString displayText;
    switch (backupArgs.getBackupRecurrence()) {
    case ScheduleRecurrence::ONCE:
        if (date.isDateComplete()) {
                displayText = "Once in " + formatDate(date.getDate(BackupArgs::DAYOFMONTH),
                                                      date.getDate(BackupArgs::MONTH),
                                                      date.getDate(BackupArgs::YEAR),
                                                      date.getDate(BackupArgs::HOUR),
                                                      date.getDate(BackupArgs::MINUTE));
        }
        break;
    case ScheduleRecurrence::MONTHLY:
        if (date.getDate(BackupArgs::DAYOFMONTH) != -1) {
                displayText = QString("Every month in %1 day at %2")
                                  .arg(addZero(date.getDate(BackupArgs::DAYOFMONTH)),
                                       getTimeString(date.getDate(BackupArgs::HOUR),
                                                     date.getDate(BackupArgs::MINUTE)));
        }
        break;
    case ScheduleRecurrence::WEEKLY: {
        int dayValue = date.getDate(BackupArgs::DAYOFWEEK);
        const QString& dayName = dayOfWeekMap.value(dayValue, "");
        if (!dayName.isEmpty()) {
                displayText = QString("Every %1 at %2").arg(dayName,
                                                            getTimeString(date.getDate(BackupArgs::HOUR),
                                                                          date.getDate(BackupArgs::MINUTE)));
        } else {
                // TODO: handle error
        }
        break;
    }
    case ScheduleRecurrence::DAILY:
        displayText = QString("Every day at %1").arg(getTimeString(date.getDate(BackupArgs::HOUR),
                                                                   date.getDate(BackupArgs::MINUTE)));
        break;
    default:
        // TODO: Handle this case as you find appropriate
        break;
    }

    if (displayText.isEmpty()) {
        ui->chosenDateL->setText("Chosen date is...");
    } else {
        ui->chosenDateL->setText(displayText);
    }
}


void BackupScreen::createBackupBclicked() {
    auto type = backupArgs.getBackupType();
    auto sourcePath = backupArgs.getSourcePath();
    auto year = backupArgs.getDate(BackupArgs::YEAR);
    auto month = backupArgs.getDate(BackupArgs::MONTH);
    auto dayOfMonth = backupArgs.getDate(BackupArgs::DAYOFMONTH);
    auto dayOfWeek = backupArgs.getDate(BackupArgs::DAYOFWEEK);
    auto hour = backupArgs.getDate(BackupArgs::HOUR);
    auto minute = backupArgs.getDate(BackupArgs::MINUTE);

    Settings& settings = Settings::getInstance();

     // delete all tasks
    for (auto& task : settings.getTaskVec()) {
        task.deleteLocal();
    }
    // delete all destinations
    for (auto& dest : settings.getDestVec()) {
        settings.remove(dest);
    }

    Destination test_dest1("DefaultDest", "D:\\Code\\sidebaricons");
    std::cout << test_dest1;
    settings.addUpdate(test_dest1);

    std::shared_ptr<OnceSchedule> once = std::make_shared<OnceSchedule>();
    once->type = BackupType::FULL;
    once->year = 2023;
    once->month = 9;
    once->day = 13;
    once->hour = 21;
    once->minute = 58;

    BackupBuilder builder;
    auto test_task1 = builder
                          .setName("Gallery")
                          .setDestinations({test_dest1})
                          .setSources({"D:\\Gallery\\backgrounds"})
                          .setSchedules({once})
//                          .setCurrentType(BackupType::FULL)
                          .buildTask();

    if (test_task1) {
        test_task1->saveLocal();

    } else {
        qDebug() << "Didnt perform saveLocal";
    }
//    test_task1->perform();

    std::vector<Task> tasks = settings.getTaskVec();
    std::vector<Destination> dests = settings.getDestVec();

    std::cout << "Tasks:" << std::endl;
    for (auto& task : tasks) {
        std::cout << task << std::endl;
    }

    std::cout << "Global destinations:" << std::endl;
    for (auto& dest : dests) {
        std::cout << dest << std::endl;
    }

////    std::optional<BackupSchedule> backupSchedule_test;

//    std::shared_ptr<Schedule> schedule;
//    std::vector<std::shared_ptr<Schedule>> schedules;
////    switch(backupArgs.getBackupRecurrence()){
////    case ScheduleRecurrence::ONCE:
//    std::shared_ptr<OnceSchedule> once = std::make_shared<OnceSchedule>();
//    once->type = BackupType::FULL;
//    once->year = 2020;
//    once->month = 11;
//    once->day = 20;
//    once->hour = 12;
//    once->minute = 35;
//    schedule = once;

//    builder.setSchedules(this->schedules);
////    case ScheduleRecurrence::MONTHLY:
////        backupSchedule_test = BackupFactory::CreateBackupSchedule<ScheduleRecurrence::MONTHLY>(
////            type, sourcePath, dayOfMonth, hour, minute);
////        break;
////    case ScheduleRecurrence::WEEKLY:
////        backupSchedule_test = BackupFactory::CreateBackupSchedule<ScheduleRecurrence::WEEKLY>(
////            type, sourcePath, dayOfWeek, hour, minute);
////        break;
////    case ScheduleRecurrence::DAILY:
////        backupSchedule_test = BackupFactory::CreateBackupSchedule<ScheduleRecurrence::DAILY>(
////            type, sourcePath, hour, minute);
////        break;
////    }

//    schedules.push_back(schedule);

//    if (!backupSchedule_test){
//        SPDLOG_ERROR("Couldn't create backupShedule_test. Error: {}",
//                     BackupFactory::ErrorCodeToString(BackupFactory::GetLastCreationError()));
//    } else {
//        HRESULT hr = backupSchedule_test->addToTaskScheduler();
//        qDebug() << "Added to Task Scheduler [BackupScreen]";
//        if (FAILED(hr)){
//            SPDLOG_ERROR("Couldn't add backupSchedule_test task. Error code: {}", hr);
//        }
//    }
}

void BackupArgs::clear() {
    for (int &arg : dateArgs) {
        arg = -1;
    }
}

bool BackupArgs::isDateComplete() {
    return dateArgs[YEAR] != -1 && dateArgs[MONTH] != -1 && dateArgs[DAYOFMONTH] != -1;
}

void BackupArgs::setDate(DateArgs type, int value) {
    dateArgs[type] = value;
}

int BackupArgs::getDate(DateArgs type) const {
    if (type >= 0 && type < dateArgs.size()) {
        return dateArgs[type];
    } else {
        SPDLOG_ERROR("Incorrect date is chosen");
        return -1; // or some default/invalid value
    }
}

void BackupArgs::setSourcePath(const std::filesystem::path& path) {
    sourceBackupDirPath = path;
}

std::filesystem::path BackupArgs::getSourcePath() {
    return sourceBackupDirPath;
}

void BackupArgs::setBackupType(BackupType type) {
    backupType = type;
}

BackupType BackupArgs::getBackupType() {
    return backupType;
}

void BackupArgs::setBackupRecurrence(ScheduleRecurrence recurrence) {
    this->backupRecurrence = recurrence;
}

ScheduleRecurrence BackupArgs::getBackupRecurrence() {
    return backupRecurrence;
}

QString BackupScreen::addZero(int number) {
    if (number >= 0 && number <= 9) {
        return QString("%1").arg(number, 2, 10, QChar('0'));
    }
    return QString("%1").arg(number);
}

