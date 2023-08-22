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
//    QFile mainWindowFile(":/styles/mainWindow.css");
//    mainWindowFile.open(QFile::ReadOnly);
//    QString mainWindowStyleSheet = QString::fromUtf8(mainWindowFile.readAll());
//    ui->centralMainWidget->setStyleSheet(mainWindowStyleSheet);

//    QFile file(":/styles/mainWindow.css");
//    if (file.open(QFile::ReadOnly | QFile::Text)) {
//        QTextStream stream(&file);
//        qApp->setStyleSheet(stream.readAll());
//    }

    QFile file(":/styles/mainWindow.css");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);

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


    ui->homeScreenB->installEventFilter(this);
    ui->backupScreenB->installEventFilter(this);
    ui->recoverScreenB->installEventFilter(this);
    ui->historyScreenB->installEventFilter(this);
    ui->accountScreenB->installEventFilter(this);
    ui->settingsScreenB->installEventFilter(this);
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

// TODO: rewrite this function
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Enter)
    {
        if (watched == ui->homeScreenB)
        {
            QPushButton *btn = qobject_cast<QPushButton *>(watched);
            if (btn)
            {
                btn->setIcon(QIcon(":/resources/assets/icons/sideBar/active/home_white.svg"));
            }
        }
        else if (watched == ui->backupScreenB)
        {
            QPushButton *btn = qobject_cast<QPushButton *>(watched);
            if (btn)
            {
                btn->setIcon(QIcon(":/resources/assets/icons/sideBar/active/backup_white.svg"));
            }
        }
        else if (watched == ui->recoverScreenB)
        {
            QPushButton *btn = qobject_cast<QPushButton *>(watched);
            if (btn)
            {
                btn->setIcon(QIcon(":/resources/assets/icons/sideBar/active/recover_white.svg"));
            }
        }
        else if (watched == ui->historyScreenB)
        {
            QPushButton *btn = qobject_cast<QPushButton *>(watched);
            if (btn)
            {
                btn->setIcon(QIcon(":/resources/assets/icons/sideBar/active/history_white.svg"));
            }
        }
        else if (watched == ui->accountScreenB)
        {
            QPushButton *btn = qobject_cast<QPushButton *>(watched);
            if (btn)
            {
                btn->setIcon(QIcon(":/resources/assets/icons/sideBar/active/account_white.svg"));
            }
        }
        else if (watched == ui->settingsScreenB)
        {
            QPushButton *btn = qobject_cast<QPushButton *>(watched);
            if (btn)
            {
                btn->setIcon(QIcon(":/resources/assets/icons/sideBar/active/settings_white.svg"));
            }
        }
    }
    else if (event->type() == QEvent::Leave)
    {
        if (watched == ui->homeScreenB)
        {
            QPushButton *btn = qobject_cast<QPushButton *>(watched);
            if (btn)
            {
                btn->setIcon(QIcon(":/resources/assets/icons/sideBar/notActive/home_grey.svg"));
            }
        }
        else if (watched == ui->backupScreenB)
        {
            QPushButton *btn = qobject_cast<QPushButton *>(watched);
            if (btn)
            {
                btn->setIcon(QIcon(":/resources/assets/icons/sideBar/notActive/backup_grey.svg"));
            }
        }
        else if (watched == ui->recoverScreenB)
        {
            QPushButton *btn = qobject_cast<QPushButton *>(watched);
            if (btn)
            {
                btn->setIcon(QIcon(":/resources/assets/icons/sideBar/notActive/recover_grey.svg"));
            }
        }
        else if (watched == ui->historyScreenB)
        {
            QPushButton *btn = qobject_cast<QPushButton *>(watched);
            if (btn)
            {
                btn->setIcon(QIcon(":/resources/assets/icons/sideBar/notActive/history_grey.svg"));
            }
        }
        else if (watched == ui->accountScreenB)
        {
            QPushButton *btn = qobject_cast<QPushButton *>(watched);
            if (btn)
            {
                btn->setIcon(QIcon(":/resources/assets/icons/sideBar/notActive/account_grey.svg"));
            }
        }
        else if (watched == ui->settingsScreenB)
        {
            QPushButton *btn = qobject_cast<QPushButton *>(watched);
            if (btn)
            {
                btn->setIcon(QIcon(":/resources/assets/icons/sideBar/notActive/settings_grey.svg"));
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}
