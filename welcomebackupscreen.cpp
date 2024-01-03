#include "welcomebackupscreen.h"
#include "ui_welcomebackupscreen.h"

#include "utils.h"

WelcomeBackupScreen::WelcomeBackupScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::welcomeBackupScreen)
{
    ui->setupUi(this);
    loadStyleSheet(":/styles/backupScreen.css", ui->newTaskW);
}

WelcomeBackupScreen::~WelcomeBackupScreen()
{
    delete ui;
}
