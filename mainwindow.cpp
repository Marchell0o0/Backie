#include <QDebug>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainStackedWidget = ui->mainStackedWidget;
    secondaryStackedWidget = ui->secondaryStackedWidget;

    backupScreen = new BackupScreen(this);
    signInScreen = new SignInScreen(this);

    // Add the screens to the corresponding QStackedWidgets
    mainStackedWidget->addWidget(secondaryStackedWidget); // This is where our sidebar and its associated screens are
    secondaryStackedWidget->addWidget(backupScreen); // Adding backupScreen to the secondary stacked widget
    mainStackedWidget->addWidget(signInScreen); // Adding signInScreen to the primary stacked widget directly, not inside the secondary one

    // Starting with the main application view:
    mainStackedWidget->setCurrentWidget(secondaryStackedWidget);
    secondaryStackedWidget->setCurrentWidget(backupScreen); // Show the backup screen first within the secondary widget

    connect(signInScreen, &SignInScreen::switchToMainScreen, this, &MainWindow::showMainScreen);
}

MainWindow::~MainWindow()
{
    delete backupScreen;
    delete signInScreen;
    delete ui;
}

void MainWindow::on_backupScreenB_clicked()
{
    secondaryStackedWidget->setCurrentWidget(backupScreen);
}


void MainWindow::on_accountScreenB_clicked()
{
    mainStackedWidget->setCurrentWidget(signInScreen);
}

void MainWindow::showMainScreen() {
    mainStackedWidget->setCurrentWidget(secondaryStackedWidget);
}
