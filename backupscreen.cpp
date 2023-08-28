#include <QFile>

#include "spdlog/spdlog.h"
#include "backupscreen.h"
#include "ui_backupscreen.h"
#include "dialogcalendar.h"

QDate dateOfBackup;

enum RadioButtonType {
    NowType,
    OnceDayType,
    OnceDateType,
    OnceWeekType,
    OnceMonthType,
    OnceYearType
};

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



    // set buttonType for every button
    QList<QPair<QRadioButton*, RadioButtonType>> buttonMapping = {
        {ui->nowTypeRB, NowType},
        {ui->OnceDayRB, OnceDayType},
        {ui->OnceDateRB, OnceDateType},
        {ui->OnceWeekRB, OnceWeekType},
        {ui->OnceMonthRB, OnceMonthType},
        {ui->OnceYearRB, OnceYearType}
    };

    for (const auto& pair : buttonMapping) {
        QRadioButton* radioButton = pair.first;
        RadioButtonType type = pair.second;

        radioButton->setProperty("type", type);

        connect(radioButton, &QRadioButton::toggled, this, &BackupScreen::handleRadioButtonToggle);
    }

}

BackupScreen::~BackupScreen()
{
    delete ui;
}

void BackupScreen::on_selectDateB_clicked()
{
    dialogCalendar dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QDate date = dialog.selectedDate();
        ui->chosenDateL->setText(QString("Chosen date is: %1").arg(date.toString()));
        std::vector<int> temp;
        dateComponents.clear();
        dateComponents.push_back(date.year());
        dateComponents.push_back(date.month());
        dateComponents.push_back(date.day());
        printOutDateComponents(dateComponents);
    }
    else {
        SPDLOG_DEBUG("Date not accepted");
    }
}


// implement date selection logic
void BackupScreen::handleRadioButtonToggle(bool isChecked){
    QRadioButton* button = qobject_cast<QRadioButton*>(sender());
    if (button && isChecked) {
        RadioButtonType type = static_cast<RadioButtonType>(button->property("type").toInt());
        switch (type) {
            case NowType:
                // widget all selection disabled
                ui->holderStackedWidget->setCurrentIndex(0);
                if (ui->selectDateB->isEnabled()) {
                    ui->selectDateB->setDisabled(isChecked);
                }
                ui->createBackupScheduleB->setText(QString("Make a backup"));
                break;
            case OnceDayType:
                // widget date selection disabled
                ui->holderStackedWidget->setCurrentIndex(0);
                if (ui->selectDateB->isEnabled()) {
                    ui->selectDateB->setDisabled(isChecked);
                }
                ui->createBackupScheduleB->setText(QString("Create a backup schedule"));
                break;
            case OnceDateType:
                // widget with calendar selection
                ui->holderStackedWidget->setCurrentIndex(0);
                if (!ui->selectDateB->isEnabled()) {
                    ui->selectDateB->setEnabled(isChecked);
                }
                ui->createBackupScheduleB->setText(QString("Create a backup task"));
                break;
            case OnceWeekType:
                // widget with days of week selection
                ui->holderStackedWidget->setCurrentIndex(2);
                ui->createBackupScheduleB->setText(QString("Create a backup schedule"));
                break;
            case OnceMonthType:
                // widget with month selection
                ui->holderStackedWidget->setCurrentIndex(1);
                if (ui->selectMonthCB->isVisible()) {
                    ui->selectMonthCB->setVisible(!isChecked);
                }
                ui->createBackupScheduleB->setText(QString("Create a backup schedule"));
                break;
            case OnceYearType:
                ui->holderStackedWidget->setCurrentIndex(1);
                if (!ui->selectMonthCB->isVisible()) {
                    ui->selectMonthCB->setVisible(isChecked);
                }
                ui->createBackupScheduleB->setText(QString("Create a backup schedule"));
                break;
        }
    }
}
void BackupScreen::printOutDateComponents(std::vector<int> dateComponents) {
    QString componentsStr;

    for (int value : dateComponents) {
        componentsStr.append(QString::number(value) + ", ");
    }

    // Removing the trailing comma and space
    if (!componentsStr.isEmpty()) {
        componentsStr.chop(2);
    }

    SPDLOG_INFO("DateComponents: {}", componentsStr.toStdString());
}
