#include <QFile>

#include "backupscreen.h"
#include "ui_backupscreen.h"
#include "dialogcalendar.h"

QDate dateOfBackup;

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

    connect(ui->nowTypeRB, &QRadioButton::toggled, this, &BackupScreen::handleRadioButtonToggle);
    connect(ui->OnceDayRB, &QRadioButton::toggled, this, &BackupScreen::handleRadioButtonToggle);
    connect(ui->OnceDateRB, &QRadioButton::toggled, this, &BackupScreen::handleRadioButtonToggle);
    connect(ui->OnceWeekRB, &QRadioButton::toggled, this, &BackupScreen::handleRadioButtonToggle);
    connect(ui->OnceMonthRB, &QRadioButton::toggled, this, &BackupScreen::handleRadioButtonToggle);
    connect(ui->OnceYearRB, &QRadioButton::toggled, this, &BackupScreen::handleRadioButtonToggle);
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
        qDebug() << "Date not accepted";
    }
}

void BackupScreen::handleRadioButtonToggle(bool isChecked){
    QRadioButton* button = qobject_cast<QRadioButton*>(sender());
    if (button && isChecked) {
        if (button == ui->nowTypeRB || button == ui->OnceDayRB) {
            // widget with calendar
            ui->holderStackedWidget->setCurrentIndex(0);
            if (ui->selectDateB->isEnabled()) {
                ui->selectDateB->setDisabled(isChecked);
            }
        }
        if (button == ui->OnceDateRB) {
            // widget with calendar
            ui->holderStackedWidget->setCurrentIndex(0);
            if (!ui->selectDateB->isEnabled()) {
                ui->selectDateB->setEnabled(isChecked);
            }
        }
        if (button == ui->OnceWeekRB) {
            // widget with days of week selection
            ui->holderStackedWidget->setCurrentIndex(2);
        }
        if (button == ui->OnceMonthRB) {
            // widget with month selection
            ui->holderStackedWidget->setCurrentIndex(1);
            if (ui->selectDayCB->isVisible()) {
                ui->selectDayCB->setVisible(!isChecked);
            }
        }
        if (button == ui->OnceYearRB){
            // widget with month and day selection
            ui->holderStackedWidget->setCurrentIndex(1);
            if (!ui->selectDayCB->isVisible()) {
                ui->selectDayCB->setVisible(isChecked);
            }
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

    qDebug() << "DateComponents:" << componentsStr;
}
