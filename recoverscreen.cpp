#include "recoverscreen.h"
#include "ui_recoverscreen.h"

RecoverScreen::RecoverScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecoverScreen)
{
    ui->setupUi(this);
}

RecoverScreen::~RecoverScreen()
{
    delete ui;
}
