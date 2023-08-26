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

    // Style for mainWindow
    QFile mainWindowFile(":/styles/mainWindow.css");
    mainWindowFile.open(QFile::ReadOnly);
    QString mainWindowStyleSheet = QString::fromUtf8(mainWindowFile.readAll());
    qApp->setStyleSheet(mainWindowStyleSheet);

    mainStackedWidget = ui->mainStackedWidget;
    secondaryStackedWidget = ui->secondaryStackedWidget;

    backupScreen = new BackupScreen(this);
    signInScreen = new SignInScreen(this);
    recoverScreen = new RecoverScreen(this);

    // Add the screens to the corresponding QStackedWidgets
    mainStackedWidget->addWidget(ui->pageMainScreen);
    secondaryStackedWidget->addWidget(backupScreen);
    secondaryStackedWidget->addWidget(recoverScreen);
    mainStackedWidget->addWidget(signInScreen);

    // Starting with the main application view:
    mainStackedWidget->setCurrentWidget(ui->pageMainScreen);
    secondaryStackedWidget->setCurrentWidget(backupScreen);

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

void MainWindow::on_backupScreenB_toggled(bool checked)
{
    secondaryStackedWidget->setCurrentWidget(backupScreen);
}

void MainWindow::on_accountScreenB_toggled(bool checked)
{
    mainStackedWidget->setCurrentWidget(signInScreen);
}

void MainWindow::on_recoverScreenB_toggled(bool checked)
{
    secondaryStackedWidget->setCurrentWidget(recoverScreen);
}
