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

    void on_backupB_clicked();

    void on_accountB_2_clicked();

private:
    Ui::MainWindow *ui;
    BackupScreen *backupScreen;
    SignInScreen *signInScreen;

    QStackedWidget *mainStackedWidget;
};
#endif // MAINWINDOW_H
