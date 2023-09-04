#include <QFile>
#include <QFileDialog>

#include "spdlog/spdlog.h"
#include "backupscreen.h"
#include "ui_backupscreen.h"
#include "dialogcalendar.h"

BackupScreen::BackupScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BackupScreen)
{
    ui->setupUi(this);

    // Style for backupScreen
    QFile backupScreenFile(":/styles/backupScreen.css");
    backupScreenFile.open(QFile::ReadOnly);
    QString backupScreenStyleSheet = QString::fromUtf8(backupScreenFile.readAll());
    ui->stackedWidget->setStyleSheet(backupScreenStyleSheet);



    // set buttonType for every Radiobutton and connect them to single function
    QList<QPair<QRadioButton*, ScheduleRecurrence>> radioButtonMapping = {
        // TODO: add NOW and ONCE A YEAR types
        {ui->OnceDateRB, ScheduleRecurrence::ONCE},
        {ui->OnceDayRB, ScheduleRecurrence::DAILY},
        {ui->OnceWeekRB, ScheduleRecurrence::WEEKLY},
        {ui->OnceMonthRB, ScheduleRecurrence::MONTHLY},
    };

    for (const auto& pair : radioButtonMapping) {
        QRadioButton* radioButton = pair.first;
        ScheduleRecurrence type = pair.second;

        radioButton->setProperty("type", static_cast<int>(type));

        connect(radioButton, &QRadioButton::toggled, this, &BackupScreen::handleRadioButtonToggle);
    }

    // set buttonType for every ComboBox and connect them to single function
    QList<QPair<QComboBox*, BackupArgs::DateArgs>> comboBoxMapping = {
        {ui->selectMonthCB, BackupArgs::MONTH},
        {ui->selectDayCB, BackupArgs::DAYOFMONTH},
        {ui->selectHourCB, BackupArgs::HOUR},
        {ui->selectMinuteCB, BackupArgs::MINUTE}
    };

    for (const auto& pair : comboBoxMapping) {
        QComboBox* comboBox = pair.first;
        BackupArgs::DateArgs type = pair.second;

        comboBox->setProperty("type", static_cast<int>(type));

        connect(comboBox, &QComboBox::currentIndexChanged, this, &BackupScreen::handleComboBoxIndexChanged);
    }
}

BackupScreen::~BackupScreen()
{
    delete ui;
}

void BackupScreen::browseForSourceFolder() {
    //TODO: decide whether to resolve symbol links
    QString chosenDir = QFileDialog::getExistingDirectory(this, tr("Choose a folder to backup"),
                                                          QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!chosenDir.isEmpty()) {
        ui->sourceEdit->setText(QString(chosenDir));
        #ifdef _WIN32  // for Windows
        // TODO: write set function
        backupArgs.setPath(std::filesystem::path(chosenDir.toStdWString()));
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
        backupArgs.setPath(std::filesystem::path(newPath.toStdString()));
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

        updateChosenDateLabel(backupArgs);
    }
    else {
        SPDLOG_DEBUG("Date not accepted");
    }
}


// implement date selection logic
void BackupScreen::handleRadioButtonToggle(bool isChecked){
    QRadioButton* button = qobject_cast<QRadioButton*>(sender());
//    if (button && !isChecked){
//        button->setChecked(true);
//    }
    if (button && isChecked) {

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
                ui->createBackupScheduleB->setText(QString("Create a backup task"));
                break;
        case ScheduleRecurrence::DAILY:
                // widget date selection disabled
                ui->holderStackedWidget->setCurrentIndex(0);
                if (ui->selectDateB->isEnabled()) {
                    ui->selectDateB->setDisabled(isChecked);
                }
                ui->createBackupScheduleB->setText(QString("Create a backup schedule"));
                break;
        case ScheduleRecurrence::WEEKLY:
                // widget with days of week selection
                ui->holderStackedWidget->setCurrentIndex(2);
                ui->createBackupScheduleB->setText(QString("Create a backup schedule"));
                break;
        case ScheduleRecurrence::MONTHLY:
                // widget with month selection
                ui->holderStackedWidget->setCurrentIndex(1);
                if (ui->selectMonthCB->isVisible()) {
                    ui->selectMonthCB->setVisible(!isChecked);
                }
                ui->createBackupScheduleB->setText(QString("Create a backup schedule"));
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

        updateChosenDateLabel(backupArgs);
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

    updateChosenDateLabel(backupArgs);
}

void BackupScreen::updateChosenDateLabel(BackupArgs date) {
    if (date.getDate(BackupArgs::HOUR) == -1 || date.getDate(BackupArgs::MINUTE) == -1) {
        ui->chosenDateL->setText(QString("Chosen date is..."));
    } else {
        switch (backupArgs.getBackupRecurrence()) {
        case ScheduleRecurrence::ONCE:
                if(date.getDate(BackupArgs::YEAR) == -1 ||
                    date.getDate(BackupArgs::MONTH) == -1 ||
                    date.getDate(BackupArgs::DAYOFMONTH) == -1) {
                    ui->chosenDateL->setText(QString("Chosen date is..."));
                } else {
                    ui->chosenDateL->setText(QString("Once in %1.%2.%3 at %4:%5")
                                                 .arg(addZero(date.getDate(BackupArgs::DAYOFMONTH)),
                                                      addZero(date.getDate(BackupArgs::MONTH)),
                                                      QString::number(date.getDate(BackupArgs::YEAR)),
                                                      addZero(date.getDate(BackupArgs::HOUR)),
                                                      addZero(date.getDate(BackupArgs::MINUTE))));

                }
                break;
        case ScheduleRecurrence::MONTHLY:
                if (date.getDate(BackupArgs::DAYOFMONTH) == -1) {
                    ui->chosenDateL->setText(QString("Chosen date is..."));
                } else {
                    ui->chosenDateL->setText(QString("Every month in %1 day at %2:%3")
                                                 .arg(addZero(date.getDate(BackupArgs::DAYOFMONTH)),
                                                      addZero(date.getDate(BackupArgs::HOUR)),
                                                      addZero(date.getDate(BackupArgs::MINUTE))));
                }


                break;
        case ScheduleRecurrence::WEEKLY:
                if (date.getDate(BackupArgs::DAYOFWEEK) == -1) {
                    ui->chosenDateL->setText(QString("Chosen date is..."));
                } else {
                    // TODO: handle this case
                }
                break;
        case ScheduleRecurrence::DAILY:
                ui->chosenDateL->setText(QString("Every day at %1:%2")
                                             .arg(addZero(date.getDate(BackupArgs::HOUR)),
                                                  addZero(date.getDate(BackupArgs::MINUTE))));
                break;
        }
    }
}

void BackupScreen::printOutDateArgs(std::vector<int> dateArgs) {
    QString componentsStr;

    for (int value : dateArgs) {
        componentsStr.append(QString::number(value) + ", ");
    }

    // Removing the trailing comma and space
    if (!componentsStr.isEmpty()) {
        componentsStr.chop(2);
    }

    SPDLOG_INFO("DateComponents: {}", componentsStr.toStdString());
}

void BackupArgs::clear() {
    for (int &arg : dateArgs) {
        arg = -1;
    }
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

void BackupArgs::setPath(const std::filesystem::path& path) {
    sourceBackupDirPath = path;
}

QString BackupArgs::getPathQString() {
    return QString::fromStdWString(sourceBackupDirPath.wstring());
}

void BackupArgs::setBackupType(BackupType type) {
    backupType = type;
}

BackupType BackupArgs::getBackupType() {
    return backupType;
}

QString BackupScreen::addZero(int number) {
    if (number >= 0 && number <= 9) {
        return QString("%1").arg(number, 2, 10, QChar('0'));
    }
    return QString("%1").arg(number);
}

void BackupArgs::setBackupRecurrence(ScheduleRecurrence recurrence) {
    backupRecurrence = recurrence;
}

ScheduleRecurrence BackupArgs::getBackupRecurrence() {
    return backupRecurrence;
}
