#include "recoveryscreen.h"
#include "ui_recoveryscreen.h"
#include "taskcard.h"

RecoveryScreen::RecoveryScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecoverScreen)
{
    ui->setupUi(this);
    TaskCard card;
//    auto *detailsLayout = new QVBoxLayout();
//    card.setContentLayout(*detailsLayout);
    QVBoxLayout *layout = static_cast<QVBoxLayout*>(ui->taskCardList->layout());
    layout->addWidget(&card);
    ui->taskCardList->setLayout(layout);
    layout->update();
    ui->taskCardList->adjustSize();
}

RecoveryScreen::~RecoveryScreen()
{
    delete ui;
}
