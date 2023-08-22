#include <QFile>

#include "backupscreen.h"
#include "ui_backupscreen.h"

BackupScreen::BackupScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BackupScreen)
{
    ui->setupUi(this);
    // Style for mainWindow
//    QFile backupScreenFile(":/styles/backupScreen.css");
//    backupScreenFile.open(QFile::ReadOnly);
//    QString backupScreenStyleSheet = QString::fromUtf8(backupScreenFile.readAll());
//    ui->stackedWidget->setStyleSheet(backupScreenStyleSheet);
}

BackupScreen::~BackupScreen()
{
    delete ui;
}
