#include <QDebug>
#include <QFile>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // If MainWindow is being initialized or shown
    QFile mainWindowFile(":/styles/mainWindow.css");
    mainWindowFile.open(QFile::ReadOnly);
    QString mainWindowStyleSheet = QString::fromUtf8(mainWindowFile.readAll());
    ui->centralMainWidget->setStyleSheet(mainWindowStyleSheet);

    mainStackedWidget = ui->mainStackedWidget;
    secondaryStackedWidget = ui->secondaryStackedWidget;

    backupScreen = new BackupScreen(this);
    signInScreen = new SignInScreen(this);
    recoverScreen = new RecoverScreen(this);

    // Add the screens to the corresponding QStackedWidgets
    mainStackedWidget->addWidget(ui->pageMainScreen); // This is where our sidebar and its associated screens are
    secondaryStackedWidget->addWidget(backupScreen); // Adding backupScreen to the secondary stacked widget
    secondaryStackedWidget->addWidget(recoverScreen); // Adding backupScreen to the secondary stacked widget
    mainStackedWidget->addWidget(signInScreen); // Adding signInScreen to the primary stacked widget directly, not inside the secondary one

    // Starting with the main application view:
    mainStackedWidget->setCurrentWidget(ui->pageMainScreen);
    secondaryStackedWidget->setCurrentWidget(backupScreen); // Show the backup screen first within the secondary widget

    connect(signInScreen, &SignInScreen::switchToMainScreen, this, &MainWindow::showMainScreen);
}

MainWindow::~MainWindow()
{
    delete backupScreen;
    delete signInScreen;
    delete ui;
}

void MainWindow::showMainScreen()
{
    mainStackedWidget->setCurrentWidget(ui->pageMainScreen);
}

void MainWindow::on_backupScreenB_clicked()
{
    secondaryStackedWidget->setCurrentWidget(backupScreen);
}

void MainWindow::on_accountScreenB_clicked()
{
    mainStackedWidget->setCurrentWidget(signInScreen);
}

void MainWindow::on_recoverScreenB_clicked()
{
    secondaryStackedWidget->setCurrentWidget(recoverScreen);
}

