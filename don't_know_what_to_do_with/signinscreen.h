#ifndef SIGNINSCREEN_H
#define SIGNINSCREEN_H

#include <QWidget>

namespace Ui {
class SignInScreen;
}

class SignInScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SignInScreen(QWidget *parent = nullptr);
    ~SignInScreen();

signals:
    void switchToMainScreen();
private slots:

    void on_goMainWindowB_clicked();

private:
    Ui::SignInScreen *ui;
};

#endif // SIGNINSCREEN_H
