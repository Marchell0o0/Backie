#include "pch.h"

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

void SignInScreen::on_goMainWindowB_clicked()
{
    emit switchToMainScreen();
}

