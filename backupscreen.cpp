#include <QFile>
#include <QFileDialog>

#include "spdlog/spdlog.h"
#include "backupscreen.h"
#include "ui_backupscreen.h"
#include "dialogcalendar.h"
#include "backupschedule.h"

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
            sourceBackupDirPath = std::filesystem::path(chosenDir.toStdWString());
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
        sourceBackupDirPath = std::filesystem::path(newPath.toStdString());
    #endif
    SPDLOG_DEBUG("Directory for backup was changed successfuly: {}", newPath.toStdString());
}

void BackupScreen::on_selectDateB_clicked()
{
    dialogCalendar dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QDate date = dialog.selectedDate();

        backupArgs.set(BackupArgs::YEAR, date.year());
        backupArgs.set(BackupArgs::MONTH, date.month());
        backupArgs.set(BackupArgs::DAYOFMONTH, date.day());

        // TODO: add selectMonthCB updating for ONCE A YEAR option
        ui->selectDayCB->setCurrentText(QString::number(backupArgs.get(BackupArgs::DAYOFMONTH)));

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

        backupRecurrence = static_cast<ScheduleRecurrence>(
            button->property("type").toInt());

        switch (backupRecurrence) {

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
                backupArgs.set(BackupArgs::DAYOFMONTH, ui->selectDayCB->currentText().toInt());
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
        backupArgs.set(BackupArgs::HOUR, ui->selectHourCB->currentText().toInt());
        backupArgs.set(BackupArgs::MINUTE, ui->selectMinuteCB->currentText().toInt());

        updateChosenDateLabel(backupArgs);
    }
}

void BackupScreen::handleComboBoxIndexChanged(int index) {
    if (index < 0) return;  // Invalid index

    QComboBox* comboBox = qobject_cast<QComboBox*>(sender());

    if (!comboBox) return;

    BackupArgs::DateArgs argType = static_cast<BackupArgs::DateArgs>(
        comboBox->property("type").toInt());

    backupArgs.set(argType, comboBox->currentText().toInt());

    updateChosenDateLabel(backupArgs);
}

void BackupScreen::updateChosenDateLabel(BackupArgs date) {
    if (date.get(BackupArgs::HOUR) == -1 || date.get(BackupArgs::MINUTE) == -1) {
        ui->chosenDateL->setText(QString("Chosen date is..."));
    } else {
        switch (backupRecurrence) {
        case ScheduleRecurrence::ONCE:
                if(date.get(BackupArgs::YEAR) == -1 ||
                    date.get(BackupArgs::MONTH) == -1 ||
                    date.get(BackupArgs::DAYOFMONTH) == -1) {
                    SPDLOG_DEBUG("It reached -1 condition");
                    ui->chosenDateL->setText(QString("Chosen date is..."));
                } else {
                    SPDLOG_DEBUG("It didnt reach -1 condition");
                    ui->chosenDateL->setText(QString("Once in %1.%2.%3 at %4:%5")
                                                 .arg(addZero(date.get(BackupArgs::DAYOFMONTH)))
                                                 .arg(addZero(date.get(BackupArgs::MONTH)))
                                                 .arg(date.get(BackupArgs::YEAR))
                                                 .arg(addZero(date.get(BackupArgs::HOUR)))
                                                 .arg(addZero(date.get(BackupArgs::MINUTE))));
                }
                break;
        case ScheduleRecurrence::MONTHLY:
                if (date.get(BackupArgs::DAYOFMONTH) == -1) {
                    ui->chosenDateL->setText(QString("Chosen date is..."));
                } else {
                    ui->chosenDateL->setText(QString("Every month in %1 day at %2:%3")
                                                 .arg(addZero(date.get(BackupArgs::DAYOFMONTH)))
                                                 .arg(addZero(date.get(BackupArgs::HOUR)))
                                                 .arg(addZero(date.get(BackupArgs::MINUTE))));
                }


                break;
        case ScheduleRecurrence::WEEKLY:
                if (date.get(BackupArgs::DAYOFWEEK) == -1) {
                    ui->chosenDateL->setText(QString("Chosen date is..."));
                } else {
                    // TODO: handle this case
                }
                break;
        case ScheduleRecurrence::DAILY:
                ui->chosenDateL->setText(QString("Every day at %1:%2")
                                             .arg(addZero(date.get(BackupArgs::HOUR)))
                                             .arg(addZero(date.get(BackupArgs::MINUTE))));
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
    // You might also want to clear or set default values for backupType and sourceBackupDirPath
}

void BackupArgs::set(DateArgs type, int value) {
    dateArgs[type] = value;
}

int BackupArgs::get(DateArgs type) const {
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
