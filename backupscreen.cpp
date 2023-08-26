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
}

BackupScreen::~BackupScreen()
{
    delete ui;
}

void BackupScreen::on_selectDate_clicked()
{
    dialogCalendar dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QDate date = dialog.selectedDate();
        ui->chosenDateT->setText(QString("Chosen date: %1").arg(date.toString()));
//        return date;
    }
    else {
        qDebug() << "Date not accepted";
    }
}

