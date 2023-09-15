#ifndef WELCOMEBACKUPSCREEN_H
#define WELCOMEBACKUPSCREEN_H

#include <QWidget>

namespace Ui {
class welcomeBackupScreen;
}

class WelcomeBackupScreen : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeBackupScreen(QWidget *parent = nullptr);
    ~WelcomeBackupScreen();

private:
    Ui::welcomeBackupScreen *ui;
};

#endif // WELCOMEBACKUPSCREEN_H
