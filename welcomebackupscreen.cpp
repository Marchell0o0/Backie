#include "welcomebackupscreen.h"
#include "ui_welcomebackupscreen.h"

#include <iostream>

#include "taskcard.h"
#include "utils.h"
//#include "settings.h"


WelcomeBackupScreen::WelcomeBackupScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::welcomeBackupScreen)
{
    ui->setupUi(this);
    loadStyleSheet(":/styles/backupScreen.css", ui->newTaskW);

//    Settings& settings = Settings::getInstance();
//    std::vector<Task> tasks = settings.getTaskVec();
//    std::cout << "Tasks:" << std::endl;
//    for (auto& task : tasks) {
//        std::cout << task << std::endl;
//    }

//    TaskCard *card1 = new TaskCard(this);
//    TaskCard *card2 = new TaskCard(this);
//    TaskCard *card3 = new TaskCard(this);
    QLayout *layout = ui->scrollAreaWidgetContents->layout();
    QSpacerItem *item = new QSpacerItem(30, 30, QSizePolicy::Expanding, QSizePolicy::Fixed);
    for (int i = 0; i < 10; i++) {
        TaskCard *card = new TaskCard(this);
        static_cast<QVBoxLayout*>(layout)->addWidget(card);
        static_cast<QVBoxLayout*>(layout)->addSpacerItem(item);
    }


//    static_cast<QVBoxLayout*>(layout)->addWidget(card1);
//    static_cast<QVBoxLayout*>(layout)->addSpacerItem(item);
//    static_cast<QVBoxLayout*>(layout)->addWidget(card2);
//    static_cast<QVBoxLayout*>(layout)->addSpacerItem(item);
//    static_cast<QVBoxLayout*>(layout)->addWidget(card3);
//    static_cast<QVBoxLayout*>(layout)->addSpacerItem(item);
}

WelcomeBackupScreen::~WelcomeBackupScreen()
{
    delete ui;
}