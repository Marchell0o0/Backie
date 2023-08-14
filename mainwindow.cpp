#include <QDebug>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    backupScreen = new BackupScreen(this);
    signInScreen = new SignInScreen(this);

    mainStackedWidget = ui->mainStackedWidget;

    mainStackedWidget->addWidget(backupScreen);
    mainStackedWidget->addWidget(signInScreen);

    mainStackedWidget->setCurrentWidget(backupScreen);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_backupB_clicked()
{
    mainStackedWidget->setCurrentWidget(backupScreen);
    qDebug() << "Transfer to BackupScreen!";
}


void MainWindow::on_accountB_2_clicked()
{
    mainStackedWidget->setCurrentWidget(signInScreen);
    qDebug() << "Transfer to SignInScreen!";
}

