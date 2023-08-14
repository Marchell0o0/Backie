#include "backupscreen.h"
#include "ui_backupscreen.h"

BackupScreen::BackupScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BackupScreen)
{
    ui->setupUi(this);
}

BackupScreen::~BackupScreen()
{
    delete ui;
}
