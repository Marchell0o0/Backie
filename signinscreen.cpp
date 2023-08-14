#include "signinscreen.h"
#include "ui_signinscreen.h"

SignInScreen::SignInScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SignInScreen)
{
    ui->setupUi(this);
}

SignInScreen::~SignInScreen()
{
    delete ui;
}
