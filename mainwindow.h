#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>

#include "backupscreen.h"
#include "signinscreen.h"
#include "recoveryscreen.h"
#include "welcomebackupscreen.h"
//#include "taskcard.h"
#include "recoveryscreen.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void showMainScreen();

    void on_backupScreenB_toggled(bool checked);

    void on_recoverScreenB_toggled(bool checked);

    void on_accountScreenB_toggled(bool checked);

    void on_historyScreenB_clicked();

private:
    Ui::MainWindow *ui;
    BackupScreen *backupScreen;
    SignInScreen *signInScreen;
    WelcomeBackupScreen *welcomeBackupScreen;
    QStackedWidget *mainStackedWidget;
    QStackedWidget *secondaryStackedWidget;
//    TaskCard *testTaskCard;
    RecoveryScreen *recoveryScreen;

};
#endif // MAINWINDOW_H
