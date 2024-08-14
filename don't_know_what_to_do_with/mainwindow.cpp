#include "pch.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainStackedWidget = ui->mainStackedWidget;
    secondaryStackedWidget = ui->secondaryStackedWidget;

    welcomeBackupScreen = new WelcomeBackupScreen(this);
    backupScreen = new BackupScreen(this);
    signInScreen = new SignInScreen(this);
//    testTaskCard = new TaskCard(this);
    recoveryScreen = new RecoveryScreen(this);

    // Add the screens to the corresponding QStackedWidgets
    mainStackedWidget->addWidget(ui->pageMainScreen);
    secondaryStackedWidget->addWidget(welcomeBackupScreen);
    secondaryStackedWidget->addWidget(backupScreen);
//    secondaryStackedWidget->addWidget(testTaskCard);
    secondaryStackedWidget->addWidget(recoveryScreen);
    mainStackedWidget->addWidget(signInScreen);

    // Starting with the main application view:
    mainStackedWidget->setCurrentWidget(ui->pageMainScreen);
    secondaryStackedWidget->setCurrentWidget(welcomeBackupScreen);

    connect(signInScreen, &SignInScreen::switchToMainScreen, this, &MainWindow::showMainScreen);

}

MainWindow::~MainWindow()
{
    delete backupScreen;
    delete signInScreen;
//    delete testTaskCard;
    delete ui;
}

void MainWindow::showMainScreen()
{
    mainStackedWidget->setCurrentWidget(ui->pageMainScreen);
}

void MainWindow::on_backupScreenB_toggled(bool checked)
{
    secondaryStackedWidget->setCurrentWidget(welcomeBackupScreen);
}

void MainWindow::on_accountScreenB_toggled(bool checked)
{
    mainStackedWidget->setCurrentWidget(signInScreen);
}

void MainWindow::on_recoverScreenB_toggled(bool checked)
{
    secondaryStackedWidget->setCurrentWidget(backupScreen);
}

void MainWindow::on_historyScreenB_clicked()
{
//    secondaryStackedWidget->setCurrentWidget(testTaskCard);
    secondaryStackedWidget->setCurrentWidget(recoveryScreen);
}
