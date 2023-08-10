#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_hiButton_clicked()
{
    qDebug() << "Hi there";
}


void MainWindow::on_sendMessage_clicked()
{
     qDebug() << "Message was successfully sent";
}

