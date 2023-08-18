#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include "backupscreen.h"
#include "signinscreen.h"

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

//    void on_accountScreenB_clicked();

//    void on_backupScreenB_clicked();

    void on_backupScreenB_clicked();

    void on_accountScreenB_clicked();

    void showMainScreen();

private:
    Ui::MainWindow *ui;
    BackupScreen *backupScreen;
    SignInScreen *signInScreen;
    QStackedWidget *mainStackedWidget;
    QStackedWidget *secondaryStackedWidget;
};
#endif // MAINWINDOW_H
